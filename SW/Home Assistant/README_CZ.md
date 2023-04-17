# Jak uvést do chodu displej

1. Přidat text z configuration.yaml do configuration.yaml v Home Assistant (není potřeba nic měnit na vyzkoušení, jinak je nezbytné upravit parametry podle sebe).
   
2. Vložit složku fonts do esphome/config.

3. Přidat displej do esphome a Home Assistant.
   
4. Zkopírovat text z espink.yaml do yaml konfigurace zařízení  a upravit nezbytné parametry. **Jméno zařízení se musí shodovat se jménem předem nakonfigurovaného zařízení.**

5. Nahrát program do zařízení. Odteď už nebude možné nahrát kód do zařízení bezdrátově, pouze lokálně, protože zařízení tráví většinu času v režimu spánku.

# Co kód dělá?

![esphome](../../img/esphome.jpg)

Zobrazí data z [TMEP](https://tmep.cz/) na displej po jejich stažení z Home Assistant a uspí se.