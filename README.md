# Optical Energy Meter - Firmware
Arduino Skript die folgende Komponenten/Schnittstellen verwendet:

- ESP8266-Mikrocontroller
- WLAN-Modul
- Sensor (Photosensor)
- MQTT-Broker

Das Skript funktioniert wie folgt:

- Es verbindet sich mit dem WLAN-Netzwerk und dem MQTT-Broker.
- Es richtet die GPIO-Pins für die Aus- und Eingangssignale ein.
- Es beginnt mit dem Abhören von Pulsen am Eingangsstift.
- Wenn es einen Puls erkennt, startet es einen Timer.
- Wenn der Puls endet, berechnet es die Pulslänge.
- Wenn die Pulslänge innerhalb eines gültigen Bereichs liegt, wird die Pulszahl und die Gesamtzahl um eins erhöht.
- Wenn sie nicht innerhalb eines gültigen Bereichs liegt, wird der Puls ignoriert.
- Wenn eine Messung läuft, berechnet es die Leistung anhand der Pulszahl und der seit Beginn der Messung verstrichenen Zeit.
- Es veröffentlicht die Pulszahl und die Leistung auf dem MQTT-Broker.
- Es setzt die Pulszahl zurück und startet eine neue Messung.

Das Skript hat auch eine Konfigurationsdatei, mit der die folgenden Parameter festgelegt werden können:

- Ausgangspin
- Eingangspin
- Adresse des MQTT-Servers

Sobald das Skript hochgeladen und ausgeführt wird, können Sie mit einem MQTT-Client die folgenden Themen abonnieren:

`/System` - Dieses Thema enthält Informationen über das Gerät, z. B. die Firmware-Version und die Uptime.

`/Pulses` - Dieses Thema enthält Informationen über die Messung (Anzahl der Pulses)

## inbetriebnahm
- Visual Studio Code installieren
https://code.visualstudio.com/
- Erweiterung PlatformIO installieren
https://platformio.org/
- Driver Installieren
https://cdn.shopify.com/s/files/1/1509/1638/files/ch340.zip?v=1683899825
- Build Main.cpp 
- Upload to Device


