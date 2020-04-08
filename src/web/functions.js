
const flask = new CodeFlask('#editor', {
    language: 'js',
    lineNumbers: true,
    handleTabs: true
});

flask.onUpdate(compareVersions);

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
            updateIsSaved(false);
        }
        else {
            updateIsSaved(lastVersion == code);
        }
    }
}

function updateIsSaved(isSaved) {
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

function showOptionsMenu() {
    ID("options-menu").style.filter = "opacity(1)";
    ID("options-menu").style.transform = "scaleY(1)";
    ID("options-menu").style.maxHeight = "100px";
}

function hideOptionsMenu() {
    ID("options-menu").style.filter = "opacity(0)";
    ID("options-menu").style.transform = "scaleY(0)";
    ID("options-menu").style.maxHeight = "0";
}

function saveScript() {
    showOptionsMenu();
    ID("save-container").style.visibility = "visible";
}

function inputFilename() {
    setFilename(ID("filename-save").value);
    lastVersion = flask.getCode();
    updateIsSaved(true);
    hideOptionsMenu();
    ID("save-container").style.visibility = "hidden";
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
