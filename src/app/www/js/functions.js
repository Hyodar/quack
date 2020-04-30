
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

class API {
    static Resource = Object.freeze({
        STOP:       1,
        LIST:       2,
        RUN_RAW:    3,
        RUN_FILE:   4,
        OPEN:       5,
        SAVE:       6,
    });

    static bluetoothEnabled = false;
    static callbackQueue = [];

    static async enableBluetooth() {
        return new Promise((resolve, reject) => {
            bluetoothSerial.connect("MyESP32",
            () => {
                API.bluetoothEnabled = true;

                bluetoothSerial.subscribe(
                    "\0",
                    API.subscribeCallback,
                    API.bluetoothError
                );
                
                toast.show("Bluetooth Enabled!", Toast.Mode.SUCCESS);
                resolve();
            },
            (err) => {
                API.bluetoothError(err);
                reject();
            })
        });
    }

    static bluetoothError(errorMessage = "") {
        toast.show(`Bluetooth Error! ${errorMessage}`, Toast.Mode.ERROR);
    }

    static bluetoothSuccess() {
        // no-op
    }
    
    static subscribeCallback(data) {
        const callback = API.callbackQueue.pop();

        if(callback) {
            callback(data);
        }
    }

    static disableBluetooth() {
        bluetoothSerial.disconnect();
        bluetoothSerial.unsubscribe();
        API.bluetoothEnabled = false;

        toast.show("Bluetooth Disabled!", Toast.Mode.INFO);
    }

    static call(resource, request_body, ack_callback, resp_callback) {
        if(this.bluetoothEnabled) {
            bluetoothSerial.write(`\0${resource}`, API.bluetoothSuccess, API.bluetoothError);
            
            for(const value of request_body.values()) {
                bluetoothSerial.write(`${value}\0`, API.bluetoothSuccess, API.bluetoothError);
            }

            bluetoothSerial.write('\0', API.bluetoothSuccess, API.bluetoothError);

            if(ack_callback) {
                API.callbackQueue.unshift(ack_callback);
            }
            if(resp_callback) {
                API.callbackQueue.unshift(resp_callback);
            }
        }
        else {
            toast.show("Bluetooth is not enabled!", Toast.Mode.ERROR);
        }
    }
}

class Toast {
    static Mode = Object.freeze({
        SUCCESS: "#10ff0061",
        ERROR: "#ff000094",
        INFO: "#00b9ff94",
        ARDUINO_OTA: "#00ffff6b",
    });

    constructor(id) {
        this.el = ID(id);
        this.timeout = null;
        this.active = true;

        this.el.onclick = this.click.bind(this);
    }

    toggle() {
        toggle(ID("notifications-toggle"),
            () => { 
                this.active = true;
            },
            () => {
                this.active = false;
                this.clearTimeout();
                this.hide();
            },
        );
    }

    click() {
        this.el.style.right = "-60vw";
    }

    hide() {
        this.el.style.opacity = "0";
        this.el.style.width = "0px";
        this.el.style.visibility = "hidden";
        
        this.timeout = null;
    }

    reset() {
        this.el.style.right = "0px";

        this.el.style.opacity = "1";
        this.el.style.width = "";
        this.el.style.visibility = "visible";
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

    get isSaved() { ID("is-saved").innerText == "" },
    set isSaved(val) { return ID("is-saved").innerText = (val)? "" : "*" },

    get filename() { return ID("filename").innerText; },
    set filename(val) { ID("filename").innerText = val; },
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

function ID(id) {
    return document.getElementById(id);
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

    API.call(API.Resource.LIST, form, null,
             json => setOpenOptions(JSON.parse(json).dirFiles));
}

function saveScript(filename) {
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
    form.append("Script", preProcessedCode);
   
    API.call(API.Resource.SAVE, form, () => {
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
        
        API.call(API.Resource.RUN_RAW, form, () => {
            toast.show("Received command!", Toast.Mode.SUCCESS);
        })

        return;
    }
    
    if(!status.isSaved) {
        saveScript(status.filename);
        timeoutDelay = 500;
    }
    else {
        // in case it was right after SAVE was pressed
        timeoutDelay = 50;
    }
    
    // give esp a little time to save the script
    setTimeout(() => {
        form.append("Filename", status.filename);
        
        API.call(API.Resource.RUN_FILE, form, () => {
            toast.show("Received command!", Toast.Mode.SUCCESS);
        })
        
    }, timeoutDelay);
}

function stopScript() {
    const form = new FormData();

    API.call(API.Resource.STOP, form, () => {
        toast.show("Stopped script execution!", Toast.Mode.SUCCESS);
    })
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

    API.call(API.Resource.OPEN, form, null, text => {
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

function toggle(element, toggleOn, toggleOff) {
    const classes = element.classList;
    if(classes.replace("toggle-on", "toggle-off")) {
        toggleOff();
    }
    else {
        classes.replace("toggle-off", "toggle-on");
        toggleOn();
    }
}

function setTheme(newTheme) {
    editor.setOption("theme", newTheme);
}

function toggleTheme() {
    toggle(ID("theme-toggle"),
        () => setTheme("darcula"), 
        () => setTheme("default"),
    );
}

function toggleBluetooth() {
    toggle(ID("bluetooth-toggle"), 
            () => {
                API.enableBluetooth()
                .then(() => {
                    updateScriptList();
                })
                .catch(() => {
                    ID("bluetooth-toggle").classList.replace("toggle-on", "toggle-off");
                });
            },
            () => {
                API.disableBluetooth();
            }
    );
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
        scrollbarStyle: null,
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
}