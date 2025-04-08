# Autonomer Roboter mit STM32
Dieses Projekt entstand im Rahmen meines Studiums an der RWTH Aachen. Ziel war es, einen Roboter zu bauen, der autonom einfache Bewegungen ausführt.

## Funktionen
- Eigenes Design der Steuerplatine mit KiCad
- Manuelle Bestückung & SMD-Löten
- Firmware in C für STM32
- Nutzung von Modm zum Flashen
- Projektplanung mit Gantt

## Verwendete Technologien
- STM32CubeIDE
- KiCad
- modm
- Logic Analyzer, CoolTerm

## Herausforderungen
- Fehlersuche bei Signalverlusten
- Austausch defekter Komponenten
- Zeitmanagement & Teamkoordination

## Medien
<div style="border:1px solid #ccc; border-radius:8px; padding:16px; margin:16px 0; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
  <h3 style="margin-top:12px;">Steuerplatine (Top View)</h3>
  <p>Die Oberseite der selbst entwickelten Steuerplatine – entworfen mit KiCad und per Hand bestückt.</p>
  <img src="docs/media/driver.png" alt="Steuerplatine" width="100%" style="border-radius:4px;"/>
</div>

<div style="border:1px solid #ccc; border-radius:8px; padding:16px; margin:16px 0; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
  <h3 style="margin-top:12px;">3D-Ansicht der Platine</h3>
  <p>Das mit KiCad erzeugte 3D-Modell zeigt die geplante Bestückung und Bauteilausrichtung.</p>
  <img src="docs/media/3dview_driver.png" alt="3D-Ansicht" width="100%" style="border-radius:4px;"/>
</div>

<div style="border:1px solid #ccc; border-radius:8px; padding:16px; margin:16px 0; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
  <h3 style="margin-top:12px;">Bestückte Platine</h3>
  <p>Die fertige Platine nach dem manuellen Löten – bereit für den ersten Funktionstest.</p>
  <img src="docs/media/pcb_driver.png" alt="Bestückte Platine" width="100%" style="border-radius:4px;"/>
</div>

<div style="border:1px solid #ccc; border-radius:8px; padding:16px; margin:16px 0; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
  <h3 style="margin-top:12px;">Schaltplan – linker Teil</h3>
  <p>Ein Ausschnitt des Schaltplans mit Fokus auf die Spannungsversorgung und Mikrocontroller-Anbindung.</p>
  <img src="docs/media/schematic_left_driver.png" alt="Schaltplan (links)" width="100%" style="border-radius:4px;"/>
</div>

<div style="border:1px solid #ccc; border-radius:8px; padding:16px; margin:16px 0; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
  <h3 style="margin-top:12px;">Firmware-Upload via SIMA</h3>
  <p>Flashing der Firmware über Modm – der Roboter wird mit neuer Software bespielt.</p>
  <img src="docs/media/sima_robot_flashing.png" alt="SIMA Flashing" width="100%" style="border-radius:4px;"/>
</div>