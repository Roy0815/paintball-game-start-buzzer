# Ersteinrichtung

Diese Anleitung beschreibt die Ersteinrichtung über den AP-Setup-Modus
([Modus A](/architektur#modus-a-ap-setup)). Der Modus ist automatisch aktiv,
solange noch kein Setup gespeichert wurde (erster Boot nach dem Flashen).

## 1. Firmware und Dateisystem flashen

Am einfachsten direkt über den Browser: [Firmware flashen](/flash)

Beim allerersten Flashen ist noch kein Setup gespeichert. Der ESP32 startet
danach automatisch im AP-Setup-Modus.

## 2. Mit dem Setup-WLAN verbinden

Der ESP32 spannt ein offenes WLAN namens **`Paintball-Setup`** auf. Mit einem
Smartphone oder Laptop damit verbinden.

## 3. Setup-Seite öffnen

Im Browser `http://192.168.4.1/` aufrufen.

## 4. Bluetooth-Box-Namen eintragen

Den Namen der Bluetooth-Box exakt so eintragen, wie er beim normalen Pairing
sichtbar ist (Groß-/Kleinschreibung beachten).

## 5. MP3-Dateien hochladen

Eine oder mehrere MP3-Dateien auswählen und über "Ausgewählte Dateien
hochladen" übertragen. Die Gesamtgröße aller Dateien ist auf **1,5 MB**
begrenzt. Die Seite zeigt die aktuell belegte Größe an und lehnt Uploads ab,
die das Limit überschreiten würden.

In der Mapping-Tabelle (Schritt 6) wird zu jeder Datei ihre Größe angezeigt.
Über den ✕-Button am rechten Zeilenrand lässt sich eine bereits hochgeladene
Datei wieder löschen.

::: warning Warnung
Gelöschte Dateien lassen sich nicht wiederherstellen.
:::

## 6. Tastendruck-Mapping eintragen

Je nach Länge des Tastendrucks am Relais können unterschiedliche MP3s
abgespielt werden.

Für jede hochgeladene Datei erscheint automatisch eine Zeile in der
Mapping-Tabelle. Nur die **"Bis (ms)"**-Spalte muss ausgefüllt werden. Der
"Von"-Wert der jeweils nächsten Zeile wird automatisch berechnet
(`von = bis der Vorzeile + 1`). Die letzte Zeile braucht keinen "Bis"-Wert,
sie deckt alles darüber ab.

Beispiel mit 3 Dateien:

| Datei | Von (ms) | Bis (ms) |
|---|---|---|
| audio1.mp3 | 0 | 500 |
| audio2.mp3 | 501 | 1500 |
| audio3.mp3 | 1501 | unbegrenzt |

Bei nur einer hochgeladenen Datei gibt es entsprechend nur eine Zeile ohne
Begrenzung. Jeder Tastendruck löst sie aus.

Über die Pfeil-Buttons (▲/▼) am rechten Zeilenrand lässt sich die Reihenfolge
der Dateien ändern. Die Von/Bis-Grenzen bleiben dabei aufsteigend an ihre
Position gebunden. Beim Verschieben wandert also nicht der Zeitbereich mit
der Datei mit, sondern die Datei übernimmt den Zeitbereich der neuen
Position. Bereits eingetragene "Bis"-Werte bleiben der jeweiligen Datei
zugeordnet und müssen nach dem Verschieben ggf. neu geprüft werden.

## 7. Speichern

Auf **"Speichern & Neustart"** klicken. Der ESP32 schreibt Boxname, MP3-Zuordnung
und das `configured`-Flag in den NVS-Speicher und startet neu. Er landet
danach direkt im [Bluetooth-Betriebsmodus](/architektur#modus-b-bluetooth-betrieb).

## 8. Bluetooth-Verbindung prüfen

Die Bluetooth-Box muss beim ersten Verbinden und nach Software-Resets im
Kopplungsmodus sein, damit der ESP32 sich verbinden kann. Die Verbindung
sollte dann automatisch hergestellt werden.

Bei jedem weiteren Start des ESP32 reicht es dann normalerweise, wenn die
Box eingeschaltet ist. Sollte es bei einer Box nicht klappen, bitte erneut
mit dem Kopplungsmodus versuchen.

## Zurück in den Setup-Modus

Um Boxname, MP3s oder Mapping später zu ändern: Board per **EN**-Taster
(**"RESTART"** auf dem 3D-gedruckten Case) oder durch Unterbrechen der
Stromversorgung neu starten. An der Unterseite blinkt danach für 5 Sekunden
ein blaues Licht. Während dieses Licht blinkt, den **BOOT**-Taster
(**"SETUP"** auf dem 3D-gedruckten Case) drücken. Die LED sollte danach
durchgehend blau leuchten, der Setup-Modus ist dann erneut aktiv. Details:
[Entwicklung → Rückkehr in den Setup-Modus](/entwicklung#ruckkehr-in-den-setup-modus).
