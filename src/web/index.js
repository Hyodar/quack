
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