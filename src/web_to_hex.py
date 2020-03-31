
from pathlib import Path
from gzip import compress as gzip_compress

WEB_PATH = "./web"
TARGET_FILE = "./esp/web_files.h"

ARRAY_MODEL = """
#ifdef ESP_ENABLED
const u8 {array_name}[] PROGMEM = {{
#else
const u8 {array_name}[] = {{
#endif
\t{array_content}
}};
"""

CALLBACK_MODEL = '''
#define {array_name}_CALLBACK \\
server.on("{filename}", HTTP_GET, [](AsyncWebServerRequest* request) {{ \\
    reply(request, {http_code}, "{response_type}", {array_name}, sizeof({array_name})); \\
}})
'''

HEADER_MODEL = """
#ifndef WEB_FILES_H_
#define WEB_FILES_H_

#include "quack_config.h"
#include "quack_utils.h"

// ======================== GZIP Compressed Web Files ========================
{arrays}
// ============================== GET Callbacks ==============================
{callbacks}
#define GET_CALLBACKS \\
{get_callbacks}

#endif
"""

RESPONSE_TYPES = {
    ".js": "application/javascript",
    ".css": "text/css",
    ".html": "text/html"
}

def get_response_info(file):
    if file.stem.isdigit():
        http_code = file.stem
    else:
        http_code = 200

    return (http_code, RESPONSE_TYPES.get(file.suffix, "text/plain"))

def build_array(file_content):
    array_content = [
        hex(char) 
        for char in gzip_compress(file_content.encode("utf-8"))
    ]

    for i in range(9, len(array_content), 9):
        array_content[i] = f"\n\t{array_content[i]}"

    return ",\t".join(array_content).replace('\t\n', '\n')

def file_hexarray(file, array_name):
    with file.open(mode='r', encoding="utf-8") as content:
        array_content = build_array(content.read())

    return ARRAY_MODEL.format(array_name=array_name,
                              array_content=array_content)

def file_callback(file, array_name):
    http_code, response_type = get_response_info(file)

    return CALLBACK_MODEL.format(array_name=array_name,
                                 filename=file.name,
                                 http_code=http_code,
                                 response_type=response_type)

def get_array_name(file):
    if not file.stem.isdigit():
        name = file.name
    else:
        name = f"HTTP{file.name}"

    return name.replace(".", "_").upper()

def main():

    arrays = []
    callbacks = []
    get_callbacks = []

    for file in Path(WEB_PATH).glob("*"):
        array_name = get_array_name(file)

        arrays.append(file_hexarray(file, array_name))
        callbacks.append(file_callback(file, array_name))
        get_callbacks.append(f"{array_name}_CALLBACK")

    lines = HEADER_MODEL.format(
        arrays="".join(arrays),
        callbacks="".join(callbacks),
        get_callbacks="; \\\n".join(get_callbacks),
    )

    with Path(TARGET_FILE).open(mode='w', encoding="utf-8") as target_file:
        target_file.writelines(lines)


if __name__ == '__main__':
    main()