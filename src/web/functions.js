
const flask = new CodeFlask('#editor', {
    language: 'js',
    lineNumbers: true,
    handleTabs: true
});

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

function showOptionsMenu() {
    document.getElementById("options-menu").style.filter = "opacity(1)";
    document.getElementById("options-menu").style.transform = "scaleY(1)";
    document.getElementById("options-menu").style.maxHeight = "100px";
}

function hideOptionsMenu() {
    document.getElementById("options-menu").style.filter = "opacity(0)";
    document.getElementById("options-menu").style.transform = "scaleY(0)";
    document.getElementById("options-menu").style.maxHeight = "0";
}

function saveScript() {
    showOptionsMenu();
    document.getElementById("filename-container").style.visibility = "visible";
}

function saveFile() {
    hideOptionsMenu();
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
