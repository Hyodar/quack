
from string import Template as StringTemplate
from pathlib import Path
from gzip import compress as gzip_compress

WEB_PATH = "./web"
TARGET_FILE = "./esp/web_files.h"
ARRAY_TEMPLATE = StringTemplate("#ifdef ESP_ENABLED\nconst u8 ${array_name}[] PROGMEM = {\n#else\nconst u8 ${array_name}[] = {\n#endif\n\t$array\n};\n\n")
CALLBACK_TEMPLATE = StringTemplate('#define ${array_name}_CALLBACK \\\nserver.on("$filename", HTTP_GET, [](AsyncWebServerRequest* request) {\\\n\treply(request, $http_code, "$response_type", $array_name, sizeof($array_name));\\\n})\n\n');
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
    array = [hex(char) for char in gzip_compress(file_content.encode("utf-8"))]

    for i in range(10, len(array), 10):
        array[i] = '\n\t' + array[i]

    return ",\t".join(array)

def file_hexarray(file, array_name):
    with file.open(mode='r', encoding="utf-8") as content:
        array = build_array(content.read())

    return ARRAY_TEMPLATE.substitute(array_name=array_name, array=array)

def file_callback(file, array_name):
    http_code, response_type = get_response_info(file)

    return CALLBACK_TEMPLATE.substitute(array_name=array_name,
                                        filename=file.name,
                                        http_code=http_code,
                                        response_type=response_type)

def main():
    lines = [
        "\n",
        "#ifndef WEB_FILES_H_\n",
        "#define WEB_FILES_H_\n",
        "\n",
        '#include "quack_config.h"\n',
        '#include "quack_utils.h"\n',
        "\n",
    ]

    callbacks = []

    for file in Path(WEB_PATH).glob("*"):
        if file.stem.isdigit():
            name = f"HTTP{file.name}"
        else:
            name = file.name

        array_name = name.replace('.', '_').upper()

        lines.append(file_hexarray(file, array_name));
        lines.append(file_callback(file, array_name));
        callbacks.append(f"{array_name}_CALLBACK");
    
    callbacks = '; \\\n'.join(callbacks)
    lines.append(f"#define GET_CALLBACKS \\\n{callbacks}\n\n")
    lines.append("#endif");

    with Path(TARGET_FILE).open(mode='w', encoding="utf-8") as target_file:
        target_file.writelines(lines)


if __name__ == '__main__':
    main()