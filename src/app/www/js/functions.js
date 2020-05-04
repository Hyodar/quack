
/*****************************************************************************
 * Globals
*****************************************************************************/

const commands = [
    "DEFAULTDELAY", "DELAY", "STRING", "DISPLAY", "REPEAT", "KEYS",
];

const keys = [
    "CTRL", "CONTROL", "SHIFT", "ALT", "WINDOWS",
    "GUI", "ENTER", "MENU", "APP", "DELETE", "HOME",
    "INSERT", "PAGEUP", "PAGEDOWN", "UP", "UPARROW",
    "DOWN", "DOWNARROW", "LEFT", "LEFTARROW", "RIGHT",
    "RIGHTARROW", "TAB", "END", "ESC", "ESCAPE", "SPACE",
    "PAUSE", "BREAK", "CAPSLOCK", "NUMLOCK", "PRINTSCREEN",
    "SCROLLLOCK",
];

const translateKey = {
    "CONTROL": "CTRL",
    "UP": "UPARROW",
    "DOWN": "DOWNARROW",
    "LEFT": "LEFTARROW",
    "RIGHT": "RIGHTARROW",
    "ESCAPE": "ESC",
    "BREAK": "PAUSE",
};

const translate = {
    "DEFAULT_DELAY": "DEFAULTDELAY",
    "REPLAY": "REPEAT",
};

async function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function ID(id) {
    return document.getElementById(id);
}

class ToggleButton {

    static State = Object.freeze({
        ON:     true,
        OFF:    false,
    });

    constructor(id, callbackOn, callbackOff, state=ToggleButton.State.OFF) {
        this.id = id;
        this.callbackOn = callbackOn;
        this.callbackOff = callbackOff;
        this.state = state;
    }

    get element() {
        return ID(this.id);
    }

    on(runCallback=true) {
        this.element.classList.replace("toggle-off", "toggle-on");
        this.state = ToggleButton.State.ON;
        if(runCallback) this.callbackOn();
    }

    off(runCallback=true) {
        this.element.classList.replace("toggle-on", "toggle-off");
        this.state = ToggleButton.State.OFF;
        if(runCallback) this.callbackOff();
    }

    toggle() {
        if(this.state) {
            this.off();
        }
        else {
            this.on();
        }
    }
}

class BluetoothAPI {

    constructor() {
        this.isEnabled = false;
        this.canSend = true;
        this.promiseQueue = [];
    }

    async sendPassword() {
        return new Promise((resolve, reject) => {
            
            navigator.notification.prompt(
                "Please enter the bluetooth password.",
                (results) => {
                    bluetoothSerial.write(results.input1, this.bluetoothSuccess.bind(this), this.error.bind(this));
                    bluetoothSerial.write('\0', this.bluetoothSuccess.bind(this), this.error.bind(this));
                },
                "Bluetooth Password",
                ["Enter"],
                "moe moe kyun"
            );

            eventSource.addEventListener("bt-right-pwd", () => {
                toast.show("Successfully logged in bluetooth serial!", Toast.Mode.SUCCESS);
                resolve();
                eventSource.removeEventListener("bt-right-pwd");
                eventSource.removeEventListener("bt-wrong-pwd");
            });

            eventSource.addEventListener("bt-wrong-pwd", () => {
                toast.show("Wrong bluetooth password!", Toast.Mode.ERROR);
                reject();
                eventSource.removeEventListener("bt-right-pwd");
                eventSource.removeEventListener("bt-wrong-pwd");
            });
        });
    }

    async _enable() {
        
        bluetoothSerial.subscribe(
            "\0",
            this.subscribeCallback,
            this.error,
        );

        return this.sendPassword();
    }

    async enable() {
        return new Promise((resolve, reject) => {
            bluetoothSerial.list((pairedDevices) => {
                const device = pairedDevices.find((el) => el.name === "MyESP32");

                if(device) {
                    bluetoothSerial.connect(device.address,
                    () => {
                        this._enable()
                            .then(() => { 
                                this.isEnabled = true; resolve();
                            })
                            .catch(() => { 
                                bluetoothSerial.unsubscribe();
                                bluetoothSerial.disconnect();
                                reject();
                            });
                    },
                    (err) => {
                        this.error(err);
                        reject();
                    });
                }
                else {
                    this.error("ESP32 not paired!");
                    reject();
                }
            },
            (err) => this.error(err)
            );
        });
    }

    disable() {
        bluetoothSerial.disconnect();
        bluetoothSerial.unsubscribe();
        this.isEnabled = false;

        toast.show("Bluetooth Disabled!", Toast.Mode.INFO);
    }

    async call(resource, requestBody) {
        return new Promise(async (resolve, reject) => {
            if(!this.canSend) {
                toast.show("Still running a command!", Toast.Mode.ERROR);
                reject();
                return;
            }
    
            this.canSend = false;
            this.promiseQueue.unshift(resolve);

            bluetoothSerial.write(`\0${resource.bluetoothId}`, this.bluetoothSuccess.bind(this), this.error.bind(this));
            
            for(const [key, val] of requestBody.entries()) {
                if(key == "File-Bytes") {
                    await this.delayedBluetoothSend(val);
                }
                else {
                    bluetoothSerial.write(`${val}\0`, this.bluetoothSuccess.bind(this), this.error.bind(this));
                }
            }

            bluetoothSerial.write('\0', this.bluetoothSuccess.bind(this), this.error.bind(this));
            
            toast.show("Sent command.", Toast.Mode.INFO);
            this.canSendBluetooth = true;
        });
    }
    
    error(errorMessage = "") {
        toast.show(`Bluetooth Error! ${errorMessage}`, Toast.Mode.ERROR);
    }

    successCallback() {
        // no-op
    }
    
    subscribeCallback(data) {
        const input = data.slice(2);

        if(data.startsWith('E|')) {
            // Event
            eventSource.bluetoothEmit(input);
        }
        else if(data.startsWith('R|')) {
            // Response
            const resolve = this.promiseQueue.pop();

            if(resolve) {
                resolve(input);
            }
        }
    }

    async delayedBluetoothSend(text) {
        const chunks = text.match(/.{1,80}/g);

        for(let i = 0; i < chunks.length; i++) {
            bluetoothSerial.write(chunks[i], this.successCallback.bind(this), this.error.bind(this));
            await sleep(300);
        }

        bluetoothSerial.write('\0', this.successCallback.bind(this), this.error.bind(this));
    }

}

class APIEventSource {
    constructor() {
        this.httpEventSource = null;
        this.listeners = {};
    }

    addEventListener(event, callback) {
        this.listeners[event] = callback;
    }

    removeEventListener(event) {
        this.listeners[event] = undefined;
    }

    bluetoothEmit(event) {
        this.listeners[event] && this.listeners[event]();
    }

    enableWiFi() {
        this.httpEventSource = new EventSource("http://ESP32.local/events");

        for([event, callback] of this.listeners.entries()) {
            this.httpEventSource.addEventListener(event, callback);
        }
    }

    disableWiFi() {
        this.httpEventSource.close();
        this.httpEventSource = null;
    }
}

class WiFiAPI {

    constructor() {
        this.isEnabled = false;
    }

    enable() {
        return new Promise((resolve, reject) => {
            WifiWizard2.getConnectedSSID()
            .then((ssid) => {
                if(ssid === "NW_ESP32") {
                    eventSource.enableWiFi();
                    toast.show("Wi-Fi module is enabled!", Toast.Mode.SUCCESS);
                    resolve();
                }
                else {
                    toast.show("Please connect yourself to NW_ESP32!", Toast.Mode.ERROR);
                    reject();
                }
            })
            .catch((err) => {
                toast.show(`Wi-Fi connection error: ${err}`, Toast.Mode.ERROR);
            });
        });
    }

    disable() {
        eventSource.disableWiFi();
    }

    async call(resource, requestBody) {
        return fetch(resource.url, {
            method: "POST",
            body: requestBody,
        }).then(response => response.text());
    }
}

class API {
    static Resource = Object.freeze({
        STOP:       { bluetoothId: 1, url: "http://ESP32.local/stop"     },
        LIST:       { bluetoothId: 2, url: "http://ESP32.local/list"     },
        RUN_RAW:    { bluetoothId: 3, url: "http://ESP32.local/run_raw"  },
        RUN_FILE:   { bluetoothId: 4, url: "http://ESP32.local/run_file" },
        OPEN:       { bluetoothId: 5, url: "http://ESP32.local/open"     },
        SAVE:       { bluetoothId: 6, url: "http://ESP32.local/save"     },
    });

    static Module = Object.freeze({
        NONE:       1,
        WIFI:       2,
        BLUETOOTH:  3,
    });

    static bluetooth = new BluetoothAPI();
    static wiFi      = new WiFiAPI();

    static bluetoothToggle = new ToggleButton(
        "bluetooth-toggle",
        () => {
            API.enableBluetooth()
               .then(() => updateScriptList())
               .catch(() => API.bluetoothToggle.off(false));
        },
        () => {
            API.disableBluetooth();
        },
        ToggleButton.State.OFF
    );

    static wiFiToggle = new ToggleButton(
        "wifi-toggle",
        () => {
            API.enableWiFi()
               .then(() => updateScriptList())
               .catch(() => API.wiFiToggle.off(false))
        },
        () => {
            API.disableWiFi();
        },
        ToggleButton.State.OFF
    );

    static activeModule = null;

    static async enableBluetooth() {
        return new Promise((resolve, reject) => {
            if(API.wiFi.isEnabled) {
                API.bluetooth.error("Disable Wi-Fi module before enabling Bluetooth!");
                reject();
                return;
            }

            API.bluetooth.enable()
                         .then(() => { API.activeModule = API.bluetooth; resolve(); })
                         .catch(reject);
        });
    }

    static disableBluetooth() {
        if(API.bluetooth.disable()) {
            API.activeModule = null;
        }
    }

    static async enableWiFi() {
        return new Promise((resolve, reject) => {
            if(API.bluetooth.isEnabled) {
                API.wiFi.error("Disable Bluetooth module before enabling Wi-Fi!");
                reject();
                return;
            }

            API.wiFi.enable()
                    .then(() => { API.activeModule = API.wiFi; resolve(); })
                    .catch(reject);
        });
    }

    static disableWiFi() {
        if(API.wiFi.disable()) {
            API.activeModule = null;
        }
    }

    static getActiveModule() {
        if(API.activeModule instanceof WiFiAPI) {
            return API.Module.WIFI;
        }
        if(API.activeModule instanceof BluetoothAPI) {
            return API.Module.BLUETOOTH;
        }
        
        return API.Module.NONE;
    }

    static async call(resource, requestBody) {
        if(!API.activeModule) {
            toast.show("Enable bluetooth or Wi-Fi module first!", Toast.Mode.ERROR);
            return new Promise((resolve, reject) => { reject(); });
        }

        return API.activeModule.call(resource, requestBody);
    }
}

class Toast {
    static Mode = Object.freeze({
        SUCCESS: "rgba(16, 255, 0, 0.6)",
        ERROR: "rgba(255, 0, 0, 0.6)",
        INFO: "rgba(0, 185, 255, 0.6)",
    });

    constructor(id) {
        this.el = ID(id);
        this.timeout = null;
        this.active = true;

        this.el.onclick = this.click.bind(this);

        this.button = new ToggleButton(
            "notifications-toggle",
            () => {
                this.active = true;
            },
            () => {
                this.active = false;
                this.clearTimeout();
                this.hide();
            },
            ToggleButton.State.ON
        );
    }

    click() {
        this.el.style.right = "-60vw";
    }

    hide() {
        Object.assign(this.el.style, {
            width:      "0px",
            opacity:    "0",
            visibility: "hidden",
        });
        
        this.timeout = null;
    }

    reset() {
        Object.assign(this.el.style, {
            width:      "",
            right:      "0px",
            opacity:    "1",
            visibility: "visible",
        });
    }

    clearTimeout() {
        if(this.timeout) {
            clearTimeout(this.timeout);
        }
    }

    show(text, mode) {
        if(!this.active) return;

        this.el.innerText = text;
        this.el.style.background = mode;

        this.reset();
        this.clearTimeout();

        this.timeout = setTimeout(this.hide.bind(this), 2000);
    }
}

const FRAME_PARAM_SIZE = 480;

const status = {
    lastVersion: "",
    hasErrors: false,

    get isSaved() { return ID("is-saved").innerText == ""; },
    set isSaved(val) { ID("is-saved").innerText = (val)? "" : "*" },

    get filename() { return ID("filename").innerText; },
    set filename(val) { ID("filename").innerText = val; },

    theme: new ToggleButton(
        "theme-toggle",
        () => editor.setOption("theme", "darcula"),
        () => editor.setOption("theme", "default"),
        ToggleButton.State.ON
    ),
};

/*****************************************************************************
 * Functions
*****************************************************************************/

function compareVersions(code) {
    if(status.lastVersion.length != code.length) {
        status.isSaved = false;
    }
    else {
        status.isSaved = (status.lastVersion == code);
    }
}

function setOpenOptions(newOptions) {
    const options = ID("filename-open").options;

    while(options.length) {
        options.remove();
    }

    newOptions.forEach((el) => {
        options.add(new Option(el, el));
    });
}

function showOptionsMenu(childId) {
    Object.assign(ID("options-menu").style, {
        filter: "opacity(1)",
        transform: "scaleY(1)",
        maxHeight: "100px",
    });

    hideOtherOptions(childId);
}

function hideOptionsMenu() {
    Object.assign(ID("options-menu").style, {
        filter: "opacity(0)",
        transform: "scaleY(0)",
        maxHeight: "0px",
    });

    hideOtherOptions();
}

function hideOtherOptions(childId=null) {
    Array.from(ID("options-menu").children).forEach(
        child => {
            if(child.id != childId) {
                child.style.display = "none";
            }
            else {
                child.style.display = "unset";
            }
        }
    );
}

function openFilenameInput() {
    showOptionsMenu("save-container");

    ID("filename-save").value = status.filename;
}

function preProcessCode(content) {
    let lines = content.split('\n');

    lines = lines.map((el) => {
        return el.trim();
    }).filter((el, idx) => {
        let parts = el.split(' ');
        let command = parts[0];
        
        parts = parts.filter((el) => el != "");

        if(translate[command]) {
            command = translate[command];
        }
        else if(keys.find((el) => el == command)) {
            parts = parts.map((el) => {
                if(translateKey[el]) {
                    return translateKey[el];
                }
                return el;
            });

            command = "KEYS";
            parts.unshift(command);
        }

        lines[idx] = parts.join(' ');

        return commands.find((el) => el == command);
    });

    lines.forEach((el, idx) => {
        const command = el.split(' ')[0];

        if(command == "REPEAT") {
            const temp = lines[idx - 1];
            lines[idx - 1] = lines[idx];
            lines[idx] = temp;
        }
    });

    return lines.join("\n");
}

function deProcessCode(content) {
    let lines = content.split('\n');

    for(let i = lines.length - 1; i >= 0; i--) {
        const command = lines[i].split(' ')[0];

        if(command == "REPEAT") {
            const temp = lines[i + 1];
            lines[i + 1] = lines[i];
            lines[i] = temp;
        }
    }

    return lines.join('\n');
}

function updateScriptList() {
    const form = new FormData();

    API.call(API.Resource.LIST, form)
       .then(json => { 
           alert('JSON: ' + json);
           setOpenOptions(JSON.parse(json).dirFiles);
       });
}

async function saveScript(filename) {
    if(!filename) {
        return;
    }

    if(!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    hideOptionsMenu();
    status.filename = filename;

    const preProcessedCode = preProcessCode(editor.getValue());

    const form = new FormData();
    form.append("Filename", filename);

    if(API.getActiveModule() == API.Module.BLUETOOTH) {
        form.append("File-Bytes", preProcessedCode);
    }
    else {
        const file = new File([preProcessedCode], filename, {
            type: "text/plain",
        });

        form.append("Script-File", file);
    }
   
    return API.call(API.Resource.SAVE, form)
              .then(() => {
                   status.lastVersion = editor.getValue();
                   status.isSaved = true;
                   
                   // update filename-open options
                   const options = ID("filename-open").options;
                   if(!Array.from(options).find(el => el.value == filename)) {
                       options.add(new Option(filename, filename));
                   }
              });
}

function confirmSave() {
    saveScript(ID('filename-save').value);
}

function hasLongLines(code) {
    return code.split("\n").some(line =>
        line.split(' ')[1].length >= FRAME_PARAM_SIZE
    );
}

function runScript() {
    if(status.hasErrors) {
        toast.show("There are errors in your script! Solve them before running.", Toast.Mode.ERROR);
        return;
    }

    const form = new FormData();
    const code = preProcessCode(editor.getValue());

    console.log(`Resulting Duckyscript:\n${code}`);

    if(code.length <= 450) {
        form.append("Code", code);
        
        API.call(API.Resource.RUN_RAW, form)
           .then(() => {
            toast.show("Received command!", Toast.Mode.SUCCESS);
        });

        return;
    }
    
    if(!status.isSaved) {
        saveScript(status.filename)
        .then(() => {
            API.call(API.Resource.RUN_FILE, form)
               .then(() => {
                toast.show("Received command!", Toast.Mode.SUCCESS);
            });
        })
        .catch(() => {
            toast.show("Error saving script!", Toast.Mode.ERROR);
        });
    }
    else {
        API.call(API.Resource.RUN_FILE, form)
           .then(() => {
            toast.show("Received command!", Toast.Mode.SUCCESS);
        });
    }
}

function stopScript() {
    const form = new FormData();

    API.call(API.Resource.STOP, form)
        .then(() => {
        toast.show("Stopped script execution!", Toast.Mode.SUCCESS);
    });
}

function uploadScript() {
    ID("upload-input").click();
}

function openScript() {
    let scriptName = ID("filename-open").value;
    if(!scriptName.startsWith("/")) {
        scriptName = "/" + scriptName;
    }

    const form = new FormData();
    form.append("Filename", scriptName);

    API.call(API.Resource.OPEN, form)
       .then(text => {
            editor.setValue(deProcessCode(text));
            status.lastVersion = text;
            status.isSaved = true;
            status.filename = scriptName;
    });

    hideOptionsMenu();
}

function loadScriptList() {
    showOptionsMenu("open-container");
}

function handleUpload(event) {
    const file = ID("upload-input").files[0];

    console.log(file);

    const fileReader = new FileReader();

    fileReader.onload = fileEvent => {
        const text = fileEvent.target.result;
        editor.setValue(text);
        
        status.isSaved = false;
        status.filename = file.name;
        status.lastVersion = ""
    };

    fileReader.readAsText(file, "UTF-8");
}

/*****************************************************************************
 * Main
*****************************************************************************/

function main() {

    // set file upload callback
    ID("upload-input").addEventListener("change", handleUpload);

    // CodeMirror ----------------------------------------------------------------

    editor = CodeMirror.fromTextArea(ID("editor"), {
        lineNumbers: true,
        styleActiveLine: true,
        matchBrackets: true,
        scrollPastEnd: true,
        scrollbarStyle: "null",
        lineWrapping: true,
        mode: "simplemode",
        theme: "darcula",
        gutters: ["CodeMirror-lint-markers"],
        lint: {
            onUpdateLinting: (annotationsNotSorted, annotations, cm) => {
                // when linter is done, error handling can be done here.
                // probably the run and save "Go" buttons should be disabled
                // and an error message could be displayed with alert(errMsg)

                status.hasErrors = annotations.some(line => {
                    return line.some(err => err.severity === "error")
                });
            },
            selfContain: IS_MOBILE,
        },
    });

    editor.on("change", () => compareVersions(editor.getValue()));
    // set attribute on editor to ignore slideout touch event inside it
    document.querySelector(".CodeMirror").setAttribute("data-slideout-ignore", "");
    document.querySelector(".CodeMirror").style.transition = "background 0.5s ease-in-out, color 0.5s ease-in-out";

    // Slideout ------------------------------------------------------------------

    slideout = new Slideout({
        panel: ID("panel"),
        menu: ID("menu"),
        padding: 256,
        tolerance: 70,
    });

    if(IS_MOBILE) {
        const _slideoutOpen = slideout.open.bind(slideout);
        const _slideoutClose = slideout.close.bind(slideout);

        slideout.open = () => {
            document.querySelector(".slideout-panel").style.paddingLeft = "25px";
            return _slideoutOpen();
        }

        slideout.close = () => {
            if(document.querySelector(".slideout-panel").style.paddingLeft) {
                document.querySelector(".slideout-panel").style.paddingLeft = "5px";
            }
            return _slideoutClose();
        }

        ID("sidemenu-btn").addEventListener('click', function() {
            slideout.toggle();
        });
    }

    // Toast -----------------------------------------------------------------

    toast = new Toast("toast");

    // EventSource -----------------------------------------------------------

    eventSource = new APIEventSource();

    eventSource.addEventListener("open", (e) => {
        toast.show("Connected!", Toast.Mode.SUCCESS);
    });
    
    eventSource.addEventListener("error", (e) => {
        if(e.target.readyState != EventSource.OPEN) {
            toast.show("Connection error!", Toast.Mode.ERROR);
        }
    });
    
    eventSource.addEventListener("received", () => {
        toast.show("Received script!", Toast.Mode.INFO);
    });
    
    eventSource.addEventListener("finished", () => {
        toast.show("Finished executing!", Toast.Mode.SUCCESS);
    });

    eventSource.addEventListener("bt-max-pwd-tries", () => {
        toast.show("Maximum number of bluetooth password tries exceeded! Reset the device.", Toast.Mode.ERROR);
    });
}