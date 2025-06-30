import ezodf
import json

INPUT_ODS = "translations.ods"
OUTPUT_JSON = "translations.json"

def convert_ods_to_json(input_path, output_path):
    ezodf.config.set_table_expand_strategy('all')  # Laad alles
    doc = ezodf.opendoc(input_path)
    sheet = doc.sheets[0]  # neem eerste werkblad

    rows = list(sheet.rows())  # generator naar lijst omzetten
    headers = [cell.value for cell in rows[0]]  # eerste rij als headers
    translations = {}

    total_rows = len(rows) - 1  # excl. header
    for i, row in enumerate(rows[1:], start=1):
        values = [cell.value for cell in row]
        if not values or not values[0]:
            continue  # sla lege rijen of lege key over
        key = str(values[0]).strip()
        translations[key] = {}

        for col_index, lang in enumerate(headers[1:], start=1):  # skip 'key'
            val = values[col_index]
            if val is not None:
                translations[key][lang] = str(val).strip()

        # Voortgang printen (elke 10 regels)
        if i % 10 == 0 or i == total_rows:
            percent = (i / total_rows) * 100
            print(f"Verwerkt {i} van {total_rows} rijen ({percent:.1f}%)")

    # Schrijf naar JSON
    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(translations, f, indent=2, ensure_ascii=False)

    print(f"✅ {output_path} is opgeslagen.")

if __name__ == "__main__":
    convert_ods_to_json(INPUT_ODS, OUTPUT_JSON)
