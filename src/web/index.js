
/*****************************************************************************
 * Globals
*****************************************************************************/

const commands = [
    "DEFAULTDELAY", "DELAY", "STRING", "DISPLAY", "REPEAT", "KEYS",
];

const keys = [
    "CTRL",     "CONTROL",      "SHIFT",    "ALT",
    "WINDOWS",  "GUI",          "ENTER",    "MENU",
    "APP",      "DELETE",       "HOME",     "INSERT",
    "PAGEUP",   "PAGEDOWN",     "UP",       "UPARROW",
    "DOWN",     "DOWNARROW",    "LEFT",     "LEFTARROW",
    "RIGHT",    "RIGHTARROW",   "TAB",      "END",
    "ESC",      "ESCAPE",       "SPACE",    "PAUSE",
    "BREAK",    "CAPSLOCK",     "NUMLOCK",  "PRINTSCREEN",
    "SCROLLLOCK",
];

const translateKey = Object.freeze({
    "CONTROL":  "CTRL",
    "UP":       "UPARROW",
    "DOWN":     "DOWNARROW",
    "LEFT":     "LEFTARROW",
    "RIGHT":    "RIGHTARROW",
    "ESCAPE":   "ESC",
    "BREAK":    "PAUSE",
});

const translate = Object.freeze({
    "DEFAULT_DELAY":    "DEFAULTDELAY",
    "REPLAY":           "REPEAT",
});

const FRAME_PARAM_SIZE = 480;

/*****************************************************************************
 * General Functions
*****************************************************************************/

/**
 * The same as document.getElementById.
 * @param {String} id 
 */
function ID(id) {
    return document.getElementById(id);
}

/*****************************************************************************
 * Classes
*****************************************************************************/

class API {
    
    /**
     * @private {Object}
     * Enumerates all API resources.
     */
    static Resource = Object.freeze({
        STOP:       "/stop",
        SAVE:       "/save",
        RUN_RAW:    "/run_raw",
        RUN_FILE:   "/run_file",
        LIST:       "/list",
        OPEN:       "/open"
    });

    /**
     * Calls an API resource.
     * @param {API.Resource} resource 
     * @param {FormData} requestBody 
     */
    static async call(resource, requestBody) {
        return fetch(resource, {
            method: "POST",
            body: requestBody,
        }).then(response =>
            (resource != API.Resource.LIST)
            ? response.text()
            : response.json()
        );
    }
}

// ---------------------------------------------------------------------------

class Toast {

    /**
     * @private {Object}
     * Background color enumeration for each Toast mode.
     */
    static Mode = Object.freeze({
        SUCCESS: "#10ff0061",
        ERROR: "#ff000094",
        INFO: "#00b9ff94",
        ARDUINO_OTA: "#00ffff6b",
    });

    /**
     * @param {String} id - DOM id of the toast element
     */
    constructor(id) {
        this.el = ID(id);
        this.timeout = null;
        this.active = true;

        this.el.onclick = this.click.bind(this);
    }

    /**
     * Toggle function for the '#notifications-toggle' button.
     */
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

    /**
     * Toast click action; Slides fast to the right.
     */
    click() {
        this.el.style.right = "-60vw";
    }

    /**
     * Hides the toast.
     */
    hide() {
        Object.assign(this.el.style, {
            width:      "0px",
            opacity:    "0",
            visibility: "hidden",
        });
        
        this.timeout = null;
    }

    /**
     * Resets the toast style.
     */
    reset() {
        Object.assign(this.el.style, {
            width:      "",
            right:      "0px",
            opacity:    "1",
            visibility: "visible",
        });
    }

    /**
     * Clears the hide timeout if there is an active timeout.
     * This prevents the former message hide function to be activated when a
     * new message is shown before the timeout ends.
     */
    clearTimeout() {
        if(this.timeout) {
            clearTimeout(this.timeout);
        }
    }

    /**
     * Show a message in the toast.
     * @param {String} text 
     * @param {Toast.Mode} mode 
     */
    show(text, mode) {
        if(!this.active) return;

        this.el.innerText = text;
        this.el.style.background = mode;

        this.reset();
        this.clearTimeout();

        this.timeout = setTimeout(this.hide.bind(this), 2000);
    }
}

/*****************************************************************************
 * Status object
*****************************************************************************/

const status = {
    lastVersion: "",
    hasErrors: false,

    get isSaved() { ID("is-saved").innerText == "" },
    set isSaved(val) { return ID("is-saved").innerText = (val)? "" : "*" },

    get filename() { return ID("filename").innerText; },
    set filename(val) { ID("filename").innerText = val; },
};

const eventSource = new EventSource("/events");
const toast = new Toast("toast");

/*****************************************************************************
 * Functions
*****************************************************************************/

/**
 * Compares the latest saved version of the script stored inside the global 
 * variable `status` with the parameter `code`.
 * @param {String} code - current editor code
 */
function compareVersions(code) {
    if(status.lastVersion.length != code.length) {
        status.isSaved = false;
    }
    else {
        status.isSaved = (status.lastVersion == code);
    }
}

// ---------------------------------------------------------------------------

/**
 * Sets the options of the DOM Select with '#filename-open' to the array of
 * filenames passed by parameter.
 * @param {Array<String>} newOptions 
 */
function setOpenOptions(newOptions) {
    const options = ID("filename-open").options;

    while(options.length) {
        options.remove();
    }

    newOptions.forEach((el) => {
        options.add(new Option(el, el));
    });
}

// ---------------------------------------------------------------------------

/**
 * Shows '#options-menu' and a child of it, passed through `childId`.
 * @param {String} childId - id of whichever child element should be visible
 */
function showOptionsMenu(childId) {
    Object.assign(ID("options-menu").style, {
        filter: "opacity(1)",
        transform: "scaleY(1)",
        maxHeight: "100px",
    });

    hideOtherOptions(childId);
}

// ---------------------------------------------------------------------------

/**
 * Hides '#options-menu' and all its children.
 */
function hideOptionsMenu() {
    Object.assign(ID("options-menu").style, {
        filter: "opacity(0)",
        transform: "scaleY(0)",
        maxHeight: "0px",
    });

    hideOtherOptions();
}

// ---------------------------------------------------------------------------

/**
 * Hides all options menu children except for the one with the id passed as
 * parameter. If childId is not set, it will hide all '#options-menu' elements.
 * @param {String} [childId]
 */
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

// ---------------------------------------------------------------------------

/**
 * Opens the filename input to save a file.
 */
function openFilenameInput() {
    showOptionsMenu("save-container");

    ID("filename-save").value = status.filename;
}

// ---------------------------------------------------------------------------

/**
 * Preprocesses code to make it standard and make processing it faster.
 * Basically:
 *  * Trim all lines
 *  * Translate commands and keys that have multiple names to one standard
 *    name.
 *  * If no command is found, replace command as 'KEYS', as it is a
 *    sequence of keys to press
 *  * Remove REM lines (comment) and whichever line doesn't have any
 *    command or the command is not recognized.
 *  * Swap the sequence of the REPEAT command: normally, in Duckyscript,
 *    REPEAT refers to the command before it. To make processing it easier,
 *    REPEAT is swapped with the line above and now refers to the command
 *    after it.
 * @param {String} content 
 */
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

// ---------------------------------------------------------------------------

/**
 * Undos some of the changes of `preProcessCode` to show the script as if in
 * normal Duckyscript.
 * The main changes are:
 *  * REPEAT command is reversed back to its normal order (see `preProcessCode`)
 *  * KEYS command is omitted
 * This, however, doesn't restore any comments.
 * @param {String} content 
 */
function deProcessCode(content) {
    let lines = content.split('\n');

    for(let i = lines.length - 1; i >= 0; i--) {
        const command = lines[i].split(' ')[0];

        if(command == "REPEAT") {
            const temp = lines[i + 1];
            lines[i + 1] = lines[i];
            lines[i] = temp;
        }
        else if(command == "KEYS") {
            const line = lines[i].split(' ');
            line.shift();
            lines[i] = line.join(' ');
        }
    }

    return lines.join('\n');
}

// ---------------------------------------------------------------------------

/**
 * Calls the API and requests a JSON with all the files inside ESP32's
 * filesystem. When it receives the response, it sets the new open options
 * with `setOpenOptions`.
 */
function updateScriptList() {
    const form = new FormData();

    form.append("Magic", 0xF00DBEEF);

    API.call(API.Resource.LIST, form)
       .then(json => setOpenOptions(json.dirFiles));
}

// ---------------------------------------------------------------------------

/**
 * Saves the current editor code as a file inside ESP32 through an API SAVE 
 * call.
 * @param {String} filename 
 */
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
    const file = new File([preProcessedCode], filename, {
        type: "text/plain",
    });

    const form = new FormData();
    form.append("Script-File", file);
   
    API.call(API.Resource.SAVE, form)
       .then(text => {
            status.lastVersion = editor.getValue();
            status.isSaved = true;
            
            // update filename-open options
            const options = ID("filename-open").options;
            if(!Array.from(options).find(el => el.value == filename)) {
                options.add(new Option(filename, filename));
            }

            console.log(`/save response: ${text}`);
       });
}

// ---------------------------------------------------------------------------

/**
 * Function which runs when the 'Go' button is clicked after inserting a
 * filename with which to save the script.
 */
function confirmSave() {
    saveScript(ID('filename-save').value);
}

// ---------------------------------------------------------------------------

/**
 * Check if any line needs to be split between multiple lines for parsing.
 * If so, the script can't be ran through the RUN_RAW API resource.
 * @param {String} code 
 */
function hasLongLines(code) {
    return code.split("\n").some(line =>
        line.split(' ')[1].length >= FRAME_PARAM_SIZE
    );
}

// ---------------------------------------------------------------------------

/**
 * Runs the script.
 * It checkes if it can be ran with RUN_RAW. If not, it runs it as a file,
 * saving the file changes first if it's needed.
 */
function runScript() {
    if(status.hasErrors) {
        toast.show("There are errors in your script! Solve them before running.", Toast.Mode.ERROR);
        return;
    }

    const form = new FormData();
    const code = preProcessCode(editor.getValue());

    console.log(`Resulting Duckyscript:\n${code}`);

    if(code.length <= 1000 && !hasLongLines(code)) {
        form.append("Code", code);
        
        API.call(API.Resource.RUN_RAW, form)
           .then(text => console.log(`/run_raw response: ${text}`));

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
        
        API.call(API.Resource.RUN_FILE, form)
           .then(text => console.log(`/run_file response: ${text}`));
        
    }, timeoutDelay);
}

// ---------------------------------------------------------------------------

/**
 * Stops the current script's execution through a call to the API STOP resource.
 */
function stopScript() {
    const form = new FormData();

    form.append("Magic", 0xF00DBEEF);

    API.call(API.Resource.STOP, form)
       .then(text => console.log(`/stop response: ${text}`));
}

// ---------------------------------------------------------------------------

/**
 * Mimicks a click to a hidden file input when a user wants to upload a script
 * through the UPLOAD button of the side menu.
 */
function uploadScript() {
    ID("upload-input").click();
}

// ---------------------------------------------------------------------------

/**
 * Loads a script that is stored inside ESP32.
 */
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

// ---------------------------------------------------------------------------

/**
 * Shows the options menu and with the '#open-container' element.
 * It runs when the OPEN button of the side menu is clicked.
 */
function loadScriptList() {
    showOptionsMenu("open-container");
}

// ---------------------------------------------------------------------------

/**
 * File upload callback for the '#upload-input' file input element.
 * @param {Event} event 
 */
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

// ---------------------------------------------------------------------------

/**
 * Toggle function for '.toggle' DOM elements.
 * Note: `element` should have 'toggle' and either 'toggle-on' or 'toggle-off'
 * in its classList.
 * @param {Element} element - DOM toggle button element
 * @param {Function} toggleOn - Callback when its state goes to on
 * @param {Function} toggleOff - Callback when its state goes to off
 */
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

// ---------------------------------------------------------------------------

/**
 * Sets CodeMirror editor theme.
 * @param {String} newTheme - theme name ("default" or "darcula")
 */
function setTheme(newTheme) {
    editor.setOption("theme", newTheme);
}

// ---------------------------------------------------------------------------

/**
 * Toggle function for the DARK MODE button in the side menu.
 */
function toggleTheme() {
    toggle(ID("theme-toggle"),
        () => setTheme("darcula"), 
        () => setTheme("default"),
    );
}

/*****************************************************************************
 * Main
*****************************************************************************/

// update script list on startup
updateScriptList();

// set file upload callback
ID("upload-input").addEventListener("change", handleUpload);

// CodeMirror ----------------------------------------------------------------

const editor = CodeMirror.fromTextArea(ID("editor"), {
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

const slideout = new Slideout({
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

// EventSource ---------------------------------------------------------------

eventSource.addEventListener("open", (e) => {
    toast.show("Connected!", Toast.Mode.SUCCESS);
}, false);

eventSource.addEventListener("error", (e) => {
    if(e.target.readyState != EventSource.OPEN) {
        toast.show("Connection error!", Toast.Mode.ERROR);
    }
}, false);

eventSource.addEventListener("received", () => {
    toast.show("Received script!", Toast.Mode.INFO);
}, false);

eventSource.addEventListener("finished", () => {
    toast.show("Finished executing!", Toast.Mode.SUCCESS);
}, false);

eventSource.addEventListener("ota", (e) => {
    toast.show(e.data, Toast.Mode.ARDUINO_OTA);
}, false);