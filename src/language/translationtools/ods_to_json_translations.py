import ezodf
import json
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))

INPUT_ODS = os.path.join(BASE_DIR, "..", "translations_src", "translations.ods")
OUTPUT_WEB_JSON = os.path.join(BASE_DIR, "translations.json")
OUTPUT_APP_JSON = os.path.join(BASE_DIR, "translations_app.json")

def convert_ods_to_json(input_path, output_web, output_app):
    ezodf.config.set_table_expand_strategy('all')
    doc = ezodf.opendoc(input_path)
    sheet = doc.sheets[0]
    rows = list(sheet.rows())

    headers = [str(cell.value).strip() if cell.value else "" for cell in rows[0]]
    translations_all = {}
    translations_app = {}

    lang_headers = headers[1:-1]  # alles tussen key en 'alls_ap'
    last_col = headers[-1]
    if last_col.lower() not in ['alls_ap', 'app', 'voor_app']:
        print("⚠️ Waarschuwing: laatste kolom heet niet 'alls_ap' maar:", last_col)

    total_rows = len(rows) - 1
    for i, row in enumerate(rows[1:], start=1):
        values = [cell.value for cell in row]
        if not values or not values[0]:
            continue

        key = str(values[0]).strip()
        trans = {}
        for col_index, lang in enumerate(lang_headers, start=1):
            val = values[col_index]
            if val:
                trans[lang] = str(val).strip()

        translations_all[key] = trans

        include_for_app = str(values[-1]).strip().lower() in ['ja', 'yes', 'true', '1']
        if include_for_app:
            translations_app[key] = trans

        if i % 10 == 0 or i == total_rows:
            percent = (i / total_rows) * 100
            print(f"Verwerkt {i} van {total_rows} rijen ({percent:.1f}%)")

    with open(output_web, "w", encoding="utf-8") as f1:
        json.dump(translations_all, f1, indent=2, ensure_ascii=False)

    with open(output_app, "w", encoding="utf-8") as f2:
        json.dump(translations_app, f2, indent=2, ensure_ascii=False)

    print(f"✅ {output_web} en {output_app} opgeslagen.")

if __name__ == "__main__":
    convert_ods_to_json(INPUT_ODS, OUTPUT_WEB_JSON, OUTPUT_APP_JSON)

