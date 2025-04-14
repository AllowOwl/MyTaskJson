#---Логика работы---

-#Определяет пути к данным
-#Загружает JSON-файлы через cURL
-#Обрабатывает и сравнивает пакеты
-#Сохраняет результат в comparison_result.json
-#Автоматически скачивает актуальные данные

#---Организация файлов---
data/
-----p10/
---------p10_packages.json
-----Sisyphus/
---------sisyphus_packages.json
-----result/
---------comparison_result.json
Структура comparison_result.json:
{
    "--stats--": {
        "p10_only_count": 0,
        "sisyphus_only_count": 0,
        "up_ver_relise_in_sisyphus_count": 0,
        "total_p10_packages": 0,
        "total_sisyphus_packages": 0
    }
    "p10_only": [
        {
        "name": "string",
        "arch": "string",
        "version": "string",
        "release": "string"
        }
    ],
    "sisyphus_only": [
        {
            "name": "string",
            "arch": "string",
            "version": "string",
            "release": "string"
        }
    ],
    "up_ver_relise_in_sisyphus": [
        {
            "name": "string",
            "arch": "string",
            "p10_version": "string-string",
            "sisyphus_version": "string-string"
        }
    ]
}