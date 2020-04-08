
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

    newOptions.forEach(el => {
        options.add(el);
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

function updateScriptList() {
    const form = new FormData();

    form.append("Magic", 0xF00DBEEF);

    fetch("/list", {
        method: "POST",
        body: form,
    }).then(
        response => response.json()
    ).then(
        obj => setOpenOptions(obj.dirFiles)
    );
}

function saveScript() {
    hideOptionsMenu();
    const filename = ID("filename-save").value;

    if(!filename) return;

    setFilename(ID("filename-save").value);
    lastVersion = flask.getCode();
    setIsSaved(true);

    const form = new FormData();

    form.append("Filename", filename);
    form.append("Code", lastVersion);
    form.append("CodeLength", lastVersion.length);
    
    fetch("/save", {
        method: "POST",
        body: form,
    }).then(response => {
        // update script list after saving
        updateScriptList();
        return response.text();
    }).then(
        text => console.log(text)
    );
}

function runScript() {
    const form = new FormData();
    const code = preProcessCode(flask.getCode());

    console.log(`Resulting Duckyscript:\n${code}`);

    form.append("Code", code);
    form.append("CodeLength", code.length);

    if(code.length >= 1000) {
        alert("You can't run a script with more than 1KB without saving.");
        alert("TODO increase this limit to - maybe - 18KB? This way we could send all of them through run_raw and preprocess it using JS");
        return;
    }

    fetch("/run_raw", {
        method: "POST",
        body: form,
    }).then(
        response => response.text()
    ).then(
        html => console.log(html)
    );
}

function stopScript() {
    const form = new FormData();

    form.append("Magic", 0xF00DBEEF);

    fetch("/stop", {
        method: "POST",
        body: form,
    }).then(
        response => response.text()
    ).then(
        html => console.log(html)
    );
}

function uploadScript() {
    ID("upload-input").click();
}

function openScript() {
    const scriptName = ID("filename-open").value;

    const form = new FormData();

    form.append("Filename", scriptName);

    fetch("/open", {
        method: "POST",
        body: form,
    }).then(
        response => response.text()
    ).then(text => {
        flask.updateCode(text);
        lastVersion = text;
        setFilename(scriptName);
    });

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