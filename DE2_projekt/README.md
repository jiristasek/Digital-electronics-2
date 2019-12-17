# Digital-Electronics-2 | Radim Dvořák 186800

[![university](https://img.shields.io/badge/university-Brno%20University%20of%20Technology-red.svg)](https://www.vutbr.cz/en/)
[![faculty](https://img.shields.io/badge/faculty-Faculty%20of%20Electrical%20Engineering%20and%20Communication-blue.svg)](https://www.fekt.vutbr.cz/)


# Semestrální projekt
#### Radim Dvořák 186800 & Jiří Šťásek 195447
##### Pozn. Kód byl psán v programu ATMEL Studio 7.0. Může proto být nutné upravení souboru Makefile tak, aby vyhovoval Vám.
## Zadání projektu
Parkovací senzory s využitím Multi function shieldu a ultrazvukového(ých) senzoru(ů) HC-SR04. Frekvence pípání podle vzdálenosti, zobrazení vzdálenosti na 7segmentovém displeji v centimetrech, indikace vzdálenosti na LED (daleko žádná, blízko všechny LED, apod.). Uvažovat nastavení limitů vzdálenosti pomoci interaktivní konzole přes UART.

## Použité součástky
| **Součástka** | **Popis** |
| ------------- | --------- |
| ATMEGA328P | Arduino Nano 
| 3x 8bitový posuvný registr | 74HC595N 
| 1x 4 Sedmisegmentový display | Common Cathode 
| Bzučák | 
| Senzor vzdálenosti | HC-SR04 
| 8x LED | 3x zelená, 3x žlutá, 2x červená 
| Tranzistor MOSFET | 
| Propojky | Různé velikosti a barvy 

## Schéma zapojení

## Realizace
### Hardware : 
Pro komunikaci se senzorem využíváme 2 piny portu B.

| **Pin senzoru** | **Pin mikrokontroléru** | **Pin na desce (Arduinu)** |
| --------------- | ----------------------- | -------------------------- |
| Trigger | PORTB1 | D9
| Echo | PORTB0 | D8

Pro aktivování senzoru je nutné přivést na Trigger pin 10 mikrosekundový pulz. Senzor potom odpovídá na pinu Echo zvednutím napěťové úrovně do logické úrovně HIGH, kterou poté udržuje po dobu odpovídající vzdálenosti, kterou změřil. Změříme-li tuto dobu, můžeme vzdálenost vypočítat podle stanovené rovnice v datasheetu senzoru.
Doporučená hranice pro buzení senzoru je dle datasheetu minimálně 60 mikrosekund.

V našem projektu jsme využili 3 posuvné registry pro adresaci sedmisegmentových displejů a také na ovládání indikačních LED diod.
Pro komunikaci s posuvnými registry využíváme port D

| **Pin registru** | **Pin mikrokontroléru** | **Pin na desce (Arduinu)** |
| ---------------- | ------------------------| -------------------------- |
| Data Pin | PORTD4 | D4
| Clock Pin | PORTD5 | D5
| Latch Pin | PORTD6 | D6

První posuvný registr je spojený s vývody displeje pro výběr displeje, druhý posuvný registr je připojen k jednotlivým segmentům displeje. Třetí registr ovládá osmici LED diod viditelně připojených na jeho výstupy.

Pro generaci signálu pro zvukovou indikaci využijeme pin D11, což je pin na jehož výstupu je připojen výstup vnitřního časovače OC2A.

### Software:

#### Rozvržení programu

| **Blok** | **Popis funkce** |
| -------- | ---------------- |
| Hlavní smyčka | Obsluha uživatelského prosředí pomocí UART 
| Timer/Counter 0 | Kontrolní jednotka. Kontroluje spínání senzoru, aktualizace displeje a LED diod. Časuje zvukové projevy zařízení
| Timer/Counter 1 | Využit jako počítadlo času pro určení vzdálenosti.
| Timer/Counter 2 | Generace signálu pro buzzer
| Pin Change Interrupt 0 | Na základě změny hladiny signálu řídí průběh měření vzdálenosti

##### Rozhraní UART

Ve hlavní smyčce bude probíhat komunikace s uživatelem přes rozhraní UART. Uživatel má následující možnosti: 
  1) Změnit nastavení minimální a maximální vzdálenosti, kdy zařízení zvukově a vizuelně (LED diody) upozorňuje uživatele na vzdálenost překážky.
  2) Změnit rychlost buzení senzoru v milisekundách. Minimálně však 60 ms
  3) Změnit frekvenci časovače OC2A a tím pádem změnit tón buzzeru na přednastavené hodnoty.
  4) Vypnout zvukovou signalizaci
  
Při každé změně hodnoty je tato hodnota uložena do vnitřní paměti (EEPROM) mikrokontroléru, takže nastavení je přeneseno i po odpojení napájecího zdroje.

##### Kontrolní jednotka

Časovač je nastaven do CTC módu (Clear Timer on Compare) na hodnotu 1 ms. Tento interrupt se pouští každou milisekundu a je rozdělen do bloků dle funkce.
  1) Blok pro aktualizaci displeje. Kontroluje, počet přerušení tak, aby byla hodnota zobrazovaného displeje inkrementována po 4 ms. Spouští odesílání dat do posuvných registrů.
  2) Blok pro spuštění měření. Při každém přerušení kontroluje aktuální časovou hodnotu od posledního spuštění. Pokud je hodnota vyšší nebo rovná, spouští další měření.
  3) Kontrola časování zvukových projevů zařízení. V závislosti na vzdálenosti spouští a zastavuje a spouští bzučení.
  
##### Měřící systém

Po vyslání budícího signálu je Echo pin vytažen do úrovně HIGH senzorem, což způsobí přerušení. V tomto přerušení spouštíme měření času. Doba, po jakou je signál na úrovni HIGH je přímo odvozená ze vzdálenosti. Když senzor opět stáhne pin na úroveň LOW, vyvolá to další přerušení, ve kterém zastavíme měření a vypočítáme vzdálenost podle vzorce.
```bash
Formula: uS / 58 = centimeters
```
Tento proces se neustále opakuje.
