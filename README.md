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
<div style="display:flex; flex-wrap:wrap; justify-content:space-between; gap:16px;">

  <div style="flex:1; min-width:300px; max-width:48%; border:1px solid #ccc; border-radius:8px; padding:16px; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
    <img src="docs/media/driver.png" alt="Steuerplatine" width="100%" style="border-radius:4px;"/>
    <h3 style="margin-top:12px;">Steuerplatine (Top View)</h3>
    <p>Oberseite der selbst entwickelten Steuerplatine mit STM32-MCU.</p>
  </div>

  <div style="flex:1; min-width:300px; max-width:48%; border:1px solid #ccc; border-radius:8px; padding:16px; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
    <img src="docs/media/3dview_driver.png" alt="3D-Ansicht" width="100%" style="border-radius:4px;"/>
    <h3 style="margin-top:12px;">3D-Ansicht der Platine</h3>
    <p>Gerenderte Darstellung der Platine in KiCad.</p>
  </div>

  <div style="flex:1; min-width:300px; max-width:48%; border:1px solid #ccc; border-radius:8px; padding:16px; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
    <img src="docs/media/pcb_driver.png" alt="Bestückte Platine" width="100%" style="border-radius:4px;"/>
    <h3 style="margin-top:12px;">Bestückte Platine</h3>
    <p>Die fertige Platine nach dem manuellen Löten.</p>
  </div>

  <div style="flex:1; min-width:300px; max-width:48%; border:1px solid #ccc; border-radius:8px; padding:16px; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
    <img src="docs/media/schematic_left_driver.png" alt="Schaltplan links" width="100%" style="border-radius:4px;"/>
    <h3 style="margin-top:12px;">Schaltplan – linker Teil</h3>
    <p>Stromversorgung und Mikrocontroller-Verbindungen im Überblick.</p>
  </div>

  <div style="flex:1; min-width:300px; max-width:48%; border:1px solid #ccc; border-radius:8px; padding:16px; box-shadow:2px 2px 6px rgba(0,0,0,0.1);">
    <img src="docs/media/sima_robot_flashing.png" alt="Flashing" width="100%" style="border-radius:4px;"/>
    <h3 style="margin-top:12px;">Firmware Flashing mit Modm</h3>
    <p>Der Roboter wird mit neuer Firmware über USB programmiert.</p>
  </div>
</div>