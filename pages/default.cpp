#include "default.h"

DefaultPage::DefaultPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    ShowText();
}

void DefaultPage::refresh()
{
    ShowText();

    WWebWidget::refresh();
}

void DefaultPage::ShowText()
{
    if (count())
    {
        for (int i = 0; i < count(); ++i)
        {
            WWidget * tmp = widget(i);
            removeWidget(tmp);
            delete tmp;
            tmp = NULL;
        }
    }

    std::string text;

    switch (session->language)
    {
        case LANG_PL:
            text = "Dane serwera:";
            text += "<ul>";
            text +=     "<li>Procesor Intel Bi Xeon E5504 8x 2.00+ GHz, 2×4 MB L2 – QPI 2×4.8 GT/sec 45nm</li>";
            text +=     "<li>RAM 24 GB DDR3</li>";
            text +=     "<li>HDD Intel SSD X25-M 2x 80 GB, SATA 2 2x 1500 GB</li>";
            text +=     "<li>Łącze 1 Gbps</li>";
            text +=     "<li>Rejestracja: Otwarta</li>";
            text +=     "<li>IP: logon.hellground.pl</li>";
            text +=     "<li>Patch: The Burning Crusade (TBC 2.4.3)</li>";
            text +=     "<li>Emulator: TrinityCore (rev 1770) + własne fixy</li>";
            text +=     "<li>Rates xp: x2</li>";
            text +=     "<li>Rates xp quest: x2</li>";
            text +=     "<li>Rates drop: x1</li>";
            text +=     "<li>Typ: PvP/PvE</li>";
            text +=     "<li>Vmapy/LoS: Tak (Spis)</li>";
            text +=     "<li>Graczy online: 2500-3000</li>";
            text +=     "<li>Główny kanał rozmów PL: /join world</li>";
            text +=     "<li>Główny kanał rozmów ENG: /join engworld</li>";
            text +=     "<li>Kanał handlu: /join handel</li>";
            text +=     "<li>Kanał szukania grupy: /join lfg</li>";
            text +=     "<li>Serwer działa od: 19 grudnia 2008r.</li>";
            text += "</ul>";
            break;
        case LANG_EN:
            text = "Server data:";
            text += "<ul>";
            text +=     "<li>Procesosr Intel Bi Xeon E5504 8x 2.00+ GHz, 2×4 MB L2 – QPI 2×4.8 GT/sec 45nm</li>";
            text +=     "<li>RAM 24 GB DDR3</li>";
            text +=     "<li>HDD Intel SSD X25-M 2x 80 GB, SATA 2 2x 1500 GB</li>";
            text +=     "<li>Port 1 Gbps</li>";
            text +=     "<li>Register: Open</li>";
            text +=     "<li>IP: logon.hellground.pl</li>";
            text +=     "<li>Patch: The Burning Crusade (TBC 2.4.3)</li>";
            text +=     "<li>Emulator: TrinityCore (rev 1770) + our own fixes</li>";
            text +=     "<li>Rates xp: x2</li>";
            text +=     "<li>Rates xp quest: x2</li>";
            text +=     "<li>Rates drop: x1</li>";
            text +=     "<li>Type: PvP/PvE</li>";
            text +=     "<li>Main channel PL: /join world</li>";
            text +=     "<li>Main channel ENG: /join engworld</li>";
            text +=     "<li>Trade Channel: /join handel</li>";
            text +=     "<li>Channel Search Groups: /join lfg</li>";
            text +=     "<li>Vmaps/LoS: Yes (List)</li>";
            text +=     "<li>Players online: 2500-3000</li>";
            text +=     "<li>The server runs since: 19 december 2008.</li>";
            text += "</ul>";
            break;
    }

    addWidget(new WText(utf8(text)));
}
