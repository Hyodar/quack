
const commands = [
    "DEFAULTDELAY", "DELAY", "STRING", "DISPLAY", "REPEAT", "KEYS"
];

const keys = [
    "CTRL", "CONTROL", "SHIFT", "ALT", "WINDOWS",
    "GUI", "ENTER", "MENU", "APP", "DELETE", "HOME",
    "INSERT", "PAGEUP", "PAGEDOWN", "UP", "UPARROW",
    "DOWN", "DOWNARROW", "LEFT", "LEFTARROW", "RIGHT",
    "RIGHTARROW", "TAB", "END", "ESC", "ESCAPE", "SPACE",
    "PAUSE", "BREAK", "CAPSLOCK", "NUMLOCK", "PRINTSCREEN",
    "SCROLLLOCK"
];

const translateKey = {
    "CONTROL": "CTRL",
    "UP": "UPARROW",
    "DOWN": "DOWNARROW",
    "LEFT": "LEFTARROW",
    "RIGHT": "RIGHTARROW",
    "ESCAPE": "ESC",
    "BREAK": "PAUSE"
};

const translate = {
    "DEFAULT_DELAY": "DEFAULTDELAY",
    "REPLAY": "REPEAT",
};

let lastVersion = "";

async function doRequest(path, method, body, typeCallback, dataCallback) {
    return fetch(path, {
        method: method,
        body: body
    }).then(
        typeCallback
    ).then(
        dataCallback
    );
}

function compareVersions(code) {
    if(lastVersion) {
        if(lastVersion.length != code.length) {
            setIsSaved(false);
        }
        else {
            setIsSaved(lastVersion == code);
        }
    }
}

function setIsSaved(isSaved) {
    ID("is-saved").innerText = (isSaved)? "" : "*";
}

function ID(id) {
    return document.getElementById(id);
}

function getFilename() {
    return ID("filename").innerText;
}

function setFilename(filename) {
    ID("filename").innerText = filename;
}

function setOpenOptions(newOptions) {
    const options = ID("filename-open").options;

    while(options.length) {
        options.remove();
    }

    newOptions.forEach((el, idx) => {
        options.add(new Option(el, el));
    });
}

function showOptionsMenu() {
    ID("options-menu").style.filter = "opacity(1)";
    ID("options-menu").style.transform = "scaleY(1)";
    ID("options-menu").style.maxHeight = "100px";
}

function hideOptionsMenu() {
    ID("options-menu").style.filter = "opacity(0)";
    ID("options-menu").style.transform = "scaleY(0)";
    ID("options-menu").style.maxHeight = "0px";

    Array.from(ID("options-menu").children).forEach(
        child => child.style.display = "none"
    );
}

function openFilenameInput() {
    showOptionsMenu();
    ID("save-container").style.display = "unset";

    ID("filename-save").value = getFilename();
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

    form.append("Magic", 0xF00DBEEF);

    doRequest("/list", "POST", form,
              response => response.json(),
              json => setOpenOptions(json.dirFiles)
    );
}

function saveScript(filename) {
    if(!filename) {
        return;
    }

    if(!filename.startsWith("/")) {
        filename = "/" + filename;
    }

    hideOptionsMenu();
    setFilename(filename);

    const preProcessedCode = preProcessCode(flask.getCode());
    const file = new File([preProcessedCode], filename, {
        type: "text/plain",
    });

    const form = new FormData();
    form.append("Script-File", file);
   
    doRequest("/save", "POST", form,
             response => {
               lastVersion = flask.getCode();
               setIsSaved(true);
               
               // update filename-open options
               const options = ID("filename-open").options;
               if(!Array.from(options).find(el => el.value == filename)) {
                   options.add(new Option(filename, filename));
               }

               return response.text();
             },
             text => console.log(`/save response: ${text}`)
    );
}

function runScript() {
    const form = new FormData();
    const code = preProcessCode(flask.getCode());

    console.log(`Resulting Duckyscript:\n${code}`);

    if(code.length <= 1000) {
        form.append("Code", code);
        
        doRequest("/run_raw", "POST", form,
            response => response.text(),
            text => console.log(`/run_raw response: ${text}`)
        );

        return;
    }

    saveScript(getFilename());
    
    // give esp a little time to save the script
    setTimeout(() => {
        form.append("Filename", getFilename());
        
        doRequest("/run_file", "POST", form,
            response => response.text(),
            text => console.log(`/run_file response: ${text}`)
        );
    }, 500);
}

function stopScript() {
    const form = new FormData();

    form.append("Magic", 0xF00DBEEF);

    doRequest("/stop", "POST", form,
              response => response.text(),
              text => console.log(`/stop response: ${text}`)
    );
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

    doRequest("/open", "POST", form,
              response => response.text(),
              text => {
                flask.updateCode(deProcessCode(text));
                lastVersion = text;
                setIsSaved(true);
                setFilename(scriptName);
              }
    );

    hideOptionsMenu();
}

function loadScriptList() {
    showOptionsMenu();
    ID("open-container").style.display = "unset";
}

function handleUpload(event) {
    const file = ID("upload-input").files[0];

    console.log(file);

    const fileReader = new FileReader();

    fileReader.onload = fileEvent => {
        const text = fileEvent.target.result;
        flask.updateCode(text);
        
        setIsSaved(false);
        setFilename(file.name);
        lastVersion = ""
    };

    fileReader.readAsText(file, "UTF-8");
}

const flask = new CodeFlask('#editor', {
    language: 'js',
    lineNumbers: true,
    handleTabs: true
});

flask.onUpdate(compareVersions);

// update script list on startup
updateScriptList();

// set file upload callback
ID("upload-input").addEventListener("change", handleUpload);