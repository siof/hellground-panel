CREATE TABLE IF NOT EXISTS LangTexts
(
    id integer NOT NULL PRIMARY KEY,
    lang_0 TEXT NOT NULL,
    lang_1 TEXT NOT NULL
) ENGINE = INNODB, CHARACTER SET utf8;

DELETE FROM LangTexts;

INSERT INTO LangTexts VALUES
(0, 'Strona główna', 'Home'),
(1, 'Zaloguj się', 'Sign In'),
(2, 'Wyloguj się', 'Sign out'),
(3, 'Zarejestruj', 'Register'),
(4, 'Odzyskiwanie hasła', 'Password recovery'),
(5, 'Zmiana hasła', 'Change password'),
(6, 'Informacje o koncie', 'Account informations'),
(7, 'Status serwera', 'Server status'),
(8, 'Dotacja', 'Donate'),
(9, 'Vote event', 'Vote event'),
(10, 'Linki', 'Links'),
(11, 'Teleport', 'Teleport'),
(12, 'Blokada IP', 'IP lock'),
(13, 'Informacje o banach', 'Ban info'),
(14, 'Lista banów', 'Ban list'),
(15, 'Informacje o mute', 'Mute info'),
(16, 'Lista mute', 'Mute list'),
(17, 'Błąd', 'Error'),
(18, 'Licencja', 'Licence'),
(19, 'Wsparcie', 'Support'),
(20, 'Postacie', 'Characters'),

(50, 'Panel GM', 'GM Panel'),
(51, 'Tickety', 'Tickets'),
(52, 'Sprawdź online', 'Online check'),
(53, 'Sprawdź frakcje', 'Faction check'),

(75, 'Nick: ', 'Nick: '),
(76, 'Poziom: ', 'Level: '),
(77, 'Klasa: ', 'Class: '),
(78, 'Całkowity czas gry: ', 'Total played time: '),
(79, 'Czas gry aktualnego poziomu: ', 'This level played time: '),
(80, 'Wybierz postać: ', 'Choose character: '),
(81, 'Rasa: ', 'Race: '),
(82, 'Koszt ostatniego resetu talentów: ', 'Last talent reset cost: '),
(83, 'Czas ostatniego resetu talentów: ', 'Last talent reset time: '),
(84, 'Postać jest online: ', 'Character is online: '),
(85, 'Ogólne informacje', 'Basic informations'),
(86, 'Questy', 'Quests'),
(87, 'Czary', 'Spells'),
(88, 'Ekwipunek', 'Inventory'),
(89, 'Znajomi', 'Friends'),
(90, 'Aktualny koszt resetu talentów: ', 'Actual talent reset cost: '),
(91, 'Data usunięcia postaci: ', 'Character deletion date: '),

(100, 'Postacie na koncie: ', 'Characters on account: '),
(101, 'Data założenia konta: ', 'Account creation date: '),
(102, 'Ostatnio zalogowany: ', 'Last time online: '),
(103, 'Ostatnie IP: ', 'Last IP: '),
(104, 'Blokada IP: ', 'IP lock: '),
(105, 'Zbanowane: ', 'Banned: '),
(106, 'Online: ', 'Online: '),
(107, 'Vote Points: ', 'Vote Points: '),
(108, 'Dodatek: ', 'Expansion: '),
(109, 'Wersja klienta: ', 'Client version: '),
(110, 'Multikonto: ', 'Multiaccount: '),
(111, 'Ostatnie odzyskiwanie hasła: ', 'Last pass restore: '),
(112, 'Login do konta: ', 'Account login: '),
(113, 'Email: ', 'Email: '),
(114, 'Numer GG: ', 'GG number: '),
(115, '<h3>Informacje o koncie:</h3>', '<h3>Account informations:</h3>'),
(116, 'Założona (zdejmij)', 'Locked (unlock)'),
(117, 'Zdjęta (załóż)', 'Unlocked (lock)'),
(118, 'IP jest zbanowane: ', 'IP is banned: '),
(119, 'Ogólne Informacje', 'Basic Informations'),
(120, 'Bany', 'Bans'),
(121, 'Mute', 'Mute'),
(122, 'Tickety', 'Tickets'),
(123, 'Historia aktywności', 'Activity history'),
(124, 'XP rate', 'XP rate'),

(130, 'Zmiana hasła', 'Change password'),
(131, 'Stare hasło', 'Old pass'),
(132, 'Nowe hasło', 'New pass'),
(133, 'Powtórz hasło', 'Repeat pass'),
(134, 'Odzyskiwanie hasła', 'Regain password'),
(135, 'Email', 'Email'),
(136, 'Numer GG', 'GG number'),

(150, 'Otwarta', 'Opened'),
(151, 'Zamknięta', 'Closed'),

(155, 'Oświadczam, że zapoznałem się i akceptuję <a href=\"http://wow.hellground.pl/regulamin-serwera/\">Regulamin Serwera</a>', 'I have read and I accept <a href=\"http://hellground.pl/showthread.php?t=8454\">Server Rules</a>'),
(156, 'Tak', 'Yes'),
(157, '<h3>HellGround - Stwórz Konto</h3>', '<h3>HellGround - Create Account</h3>'),
(158, 'Aby założyć konto musisz zaakceptować regulamin serwera.', 'You must accept server rules to register new account.'),
(159, 'Konto istnieje. Prosze wybrać inny login.', 'Account already exists. Please try another login.'),

(160, 'Login', 'Login'),
(161, 'Zbanowany od', 'Banned from'),
(162, 'Zbanowany do', 'Banned till'),
(163, 'Zbanowany przez', 'Banned by'),
(164, 'Powód', 'Reason'),
(165, 'Aktywny', 'Active'),
(166, 'Nieaktywny', 'Not active'),
(167, 'Tak', 'Yes'),
(168, 'Nie', 'No'),
(169, 'Permanentny', 'Permanent'),

(170, 'Zmutowany od', 'Muted from'),
(171, 'Zmutowany do', 'Muted till'),
(172, 'Zmutowany przez', 'Muted by'),
(173, 'Powód', 'Reason'),
(174, 'Aktywny', 'Active'),
(175, 'Nieaktywny', 'Not active'),
(176, 'Tak', 'Yes'),
(177, 'Nie', 'No'),

(185, 'Nazwa realmu: ', 'Realm name: '),
(186, 'Stan realmu: ', 'Realm state: '),
(187, 'Graczy online: ', 'Players online: '),
(188, 'Najwięcej online: ', 'Max players online: '),
(189, 'Horda: %i %% Przymierze: %i %%', 'Horde: %i %% Alliance: %i %%'),
(190, 'Realm uptime: ', 'Realm uptime: '),
(191, 'Rev: ', 'Rev: '),
(192, 'Diff: ', 'Diff: '),
(193, 'Średni diff: ', 'Avarage diff: '),
(194, '<a href=\"http://wow.hellground.pl/jak-zagrac\" target=\"_blank\"><font color=\"#ffdf81\">Jak zagrać - opis</font></a>', '<a href=\"http://en.hellground.pl/how-to-play\" target=\"_blank\"><font color=\"#ffdf81\">How to play - info</font></a>'),
(195, '<h3>Status serwerów</h3><br/>Aktualizacja co 60 sekund.', '<h3>Servers status</h3><br/>Updated every 60 seconds..'),

(200, 'Zmień', 'Change'),
(201, 'Wyczyść', 'Clear'),
(202, 'Wyślij', 'Send'),
(203, 'Zaloguj', 'Log in'),
(204, 'Zarejestruj', 'Register'),
(205, 'Teleportuj', 'Teleport'),
(206, 'Zbanowane konta', 'Accounts banned'),
(207, 'Zbanowane IP', 'IP banned'),
(208, 'Wyloguj', 'Logout'),
(209, 'Odzyskaj postać', 'Restore character'),

(225, 'Panel gracza - HellGround', 'HellGround Players Panel'),
(226, 'Logo World of Warcraft, is a registered trademark Blizzard Entertainment.<br/>© 2006-2011 Panel created by HellGround.pl ', 'Logo World of Warcraft, is a registered trademark Blizzard Entertainment.<br/>© 2006-2011 Panel created by HellGround.pl '),
(227, '<h3>O serwerze:</h3><br/>
<ul>
    <li>Serwer działa od: 19 grudnia 2008r.</li>
</ul><br/>
Sprzęt:
<ul>
    <li>Procesor Intel Bi Xeon E5504 8x 2.00+ GHz, 2×4 MB L2 – QPI 2×4.8 GT/sec 45nm</li>
    <li>RAM 24 GB DDR3</li>
    <li>HDD Intel SSD X25-M 2x 80 GB, SATA 2 2x 1500 GB</li>
    <li>Łącze 1 Gbps</li>
</ul><br/>
Dla nowych graczy:<br/>
<ul>
    <li>Rejestracja: <a href=\"#/account\"><font color=\"#00FF00\">Otwarta</font></a></li>
    <li>realmlist: set realmlist logon.hellground.pl</li>
    <li>Patch: The Burning Crusade (TBC 2.4.3)</li>
    <li>Emulator: TrinityCore + <a href=\"http://wow.hellground.pl/changelog/\">własne fixy</a></li>
    <li>Rates xp: opcjonalnie x1 - x2</li>
    <li>Rates xp quest: opcjonalnie x1 - x2</li>
    <li>Rates drop: x1</li>
    <li>Typ: PvE/PvP</li>
    <li>Vmapy/LoS: Tak ( <a href=\"http://wow.hellground.pl/vmaps-hellground/\">Spis</a> )</li>
    <li>Główny kanał rozmów PL: join world</li>
    <li>Główny kanał rozmów ENG: join engworld</li>
    <li>Kanał handlu: join handel</li>
</ul>', '<h3>About:</h3>
<ul>
    <li>Server online since: 19 december 2008r.</li>
</ul><br/>
Hardware:<br/>
<ul>
    <li>Procesor Intel Bi Xeon E5504 8x 2.00+ GHz, 2×4 MB L2 – QPI 2×4.8 GT/sec 45nm</li>
    <li>RAM 24 GB DDR3</li>
    <li>HDD Intel SSD X25-M 2x 80 GB, SATA 2 2x 1500 GB</li>
    <li>Net speed: 1 Gbps</li>
</ul><br/>
For new players:<br/>
<ul>
    <li>Registration: <a href=\"#/account\"><font color=\"#00FF00\">Available</font></a></li>
    <li>realmlist: set realmlist logon.hellground.pl</li>
    <li>Patch: The Burning Crusade (TBC 2.4.3)</li>
    <li>Emulator: TrinityCore + <a href=\"http://wowen.hellground.pl/changelog/\">own fixes</a></li>
    <li>Rates xp: optional x1 - x2</li>
    <li>Rates xp quest: optional x1 - x2</li>
    <li>Rates drop: x1</li>
    <li>Type: PvE/PvP</li>
    <li>Vmaps/LoS: Yes (<a href=\"http://wowen.hellground.pl/vmaps-hellground/\">List</a>)</li>
    <li>Main talk channel (lang: PL): join world</li>
    <li>Main talk channel (lang: ENG): join engworld</li>
    <li>Trade channel: join handel</li>
 </ul>'),
(228, 'Teleport przeniesie wybraną postać do lokacji startowej <b>danej</b> rasy. Teleportacji używać możesz w dowolnym momencie, jednak każde użycie powoduje godzinny cooldown na Heartstone’a postaci. Twoja postać nie może być zalogowana w momencie używania teleportu.<br/>
Wybierz postać, którą chcesz teleportować:', 'Teleport will move chosen character to its race starting point. You can use teleportation ** any time, but every usage sets one hour cooldown on your character’s Heartstone. Your character must be offline during teleportation.<br/>
Choose a character you want to teleport:'),
(229, 'Blokada włączona', 'IP Lock ON'),
(230, 'Blokada wyłączona', 'IP Lock OFF'),
(231, 'Bany TODO', 'Bans TODO'),
(232, 'Autostatus instancji TODO', 'Accessible instances TODO'),
(233, 'World Of Warcraft', 'World Of Warcraft'),
(234, 'World Of Warcraft: The Burning Crusade', 'World Of Warcraft: The Burning Crusade'),
(235, 'World Of Warcraft: Wrath of the Lich King', 'World Of Warcraft: Wrath of the Lich King'),
(236, 'World Of Warcraft: Cataclysm', 'World Of Warcraft: Cataclysm'),
(237, 'Aktualne IP: ', 'Current IP: '),
(238, 'Tak', 'Yes'),
(239, 'Nie', 'No'),
(240, 'Aktualne IP jest zbanowane: ', 'Current IP is banned: '),
(241, 'Hellground.pl

Dziękujemy za utworzenie konta na naszym serwerze.
<b>Login Twojego konta:</b> %s
<b>Hasło do Twojego konta</b>: %s

Zachowaj tą wiadomość na wypadek utraty loginu lub hasła – administracja nie przypomni go.

Logowanie do panelu gracza: http://panelbeta.hellground.pl.
Jak zagrać: http://wow.hellground.pl/jak-zagrac/

Życzymy miłej gry, ekipa Hellground.pl', 'Hellground.pl

Thank you for creating an account on our server.
<b>Your account’s login:</b> %s
<b>Your account’s password:</b> %s

Please, save this message in case of losing your login or password – GMT will not remind it to you.

Player’s panel: http://panelbeta.hellground.pl.
How to play: http://wowen.hellground.pl/how-to-play/

Enjoy your game, Hellground.pl GMT'),
(242, 'HellGround Server - Rejestracja konta', 'Hellground Server - Account registration'),
(243, 'Hellground.pl

Witaj,
otrzymujesz tą wiadomość, ponieważ dnia %s ktoś z adresu IP: %s próbował uzyskać nowe hasło do Twojego konta.

Twoje nowe hasło to: %s.

W celu zmiany hasła zaloguj się na http://panelbeta.hellground.pl/ wykorzystując nowe dane z tej wiadomości.

Pozdrawiamy, ekipa Hellground.pl', 'Hellground.pl

Hello,
you received this message, because on %s someone from IP address: %s tried to get a new password to your account.

Your new password: %s.

To change your password, sing in on http://panelbeta.hellground.pl/ using new data from this message.

Best wishes, Hellground.pl GMT'),
(244, 'HellGround Server - Odzyskiwanie hasła', 'Hellground Server - Password recovery'),
(245, 'Rejstracja zakończona sukcesem. Hasło zostało wysłane na podany adres email.', 'Registration complete. Password was sent to given email address.'),
(246, 'Przywracanie hasła zakończone sukcesem. Nowe hasło zostało wysłane na email.', 'Password recovery complete. New password was sent on account email address.'),
(247, 'Zmiana hasła zakończona sukcesem.', 'Password change successfull.'),
(248, 'Online', 'Online'),
(249, 'Offline', 'Offline'),
(250, '%i dni %i godzin %i minut %i sekund', '%i days %i hours %i minuts %i seconds'),
(251, 'Na pewno chcesz sie wylogować ?', 'Are you sure you want to logout ?'),
(252, 'Wystąpił błąd podczas rejestracji. Spróbuj ponownie.', 'An error occured during registration. Please try again.'),
(253, 'Teleportacja postaci zakończona powodzeniem.', 'Character teleportation successfull.'),
(254, 'Twoje konto nie zostało jeszcze zbanowane.', 'Your account was never banned.'),
(255, 'Twoje konto nie zostało jeszcze zmutowane.', 'Your account was never muted.'),
(256, 'Data', 'Date'),
(257, 'Adres IP', 'IP address'),
(258, 'Aktywność', 'Activity'),
(259, 'HG Players Panel - webowy panel dla graczy serwera HellGround<br/>
Copyright (C) 2011 HellGround Team : Siof, lukaasm,<br/><br/>

Ten program stanowi wolne oprogramowanie: możesz je rozpowszechniać<br/>
i/lub modyfikować na warunkach GNU Affero Powszechnej Licencji<br/>
Publicznej wydanej przez Free Software Foundation w wersji 3.<br/><br/>

Ten program jest rozpowszechniany z nadzieją, że jest przydatny, ale<br/>
BEZ JAKIEJKOLWIEK GWARANCJI, nawet bez domyślnej gwarancji<br/>
PRZYDATNOŚCI HANDLOWEJ ORAZ PRZYDATNOŚCI DO OKREŚLONYCH ZASTOSOWAŃ.<br/>
Przeczytaj GNU Affero Powszechną Licencję Publiczną w celu zapoznania<br/>
się ze szczegółami.<br/><br/>

Powinieneś otrzymać kopię GNU Affero Powszechnej Licencji Publiczej<br/>
wraz z programem. Jeżeli nie otrzymałeś jej, zobacz na <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.<br/><br/>

kontakt: siof@hellground.pl', 'HG Players Panel - web panel for HellGround server Players<br/>
Copyright (C) 2011 HellGround Team : Siof, lukaasm,<br/><br/>

This program is free software: you can redistribute it and/or modify<br/>
it under the terms of the GNU Affero General Public License version 3 as<br/>
published by the Free Software Foundation.<br/><br/>

This program is distributed in the hope that it will be useful,<br/>
but WITHOUT ANY WARRANTY; without even the implied warranty of<br/>
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br/>
GNU Affero General Public License for more details.<br/><br/>

You should have received a copy of the GNU Affero General Public License<br/>
along with this program.  If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.<br/><br/>

contact: siof@hellground.pl'),
(260, 'Repozytorium kodu źródłowego (zawiera także licencję): <a href=\"https://bitbucket.org/siof/hgplayerspanel\">https://bitbucket.org/siof/hgplayerspanel</a>', 'Source code repository (also contains licence): <a href=\"https://bitbucket.org/siof/hgplayerspanel\">https://bitbucket.org/siof/hgplayerspanel</a>'),
(261, '', ''),
(262, 'Pomóż rozwijać serwer Hellground poprzez dotacje oraz głosowanie na toplistach:<br/>
<a href=\"http://wow.hellground.pl/dotacja/\">Dotacja</a><br/>
<a href=\"#/support/vote-event\">Vote event</a>', 'Help expanding Hellground server through donations and voting:<br/>
<a href=\"http://wowen.hellground.pl/donate-hellground/\">Donate</a><br/>
<a href=\"#/support/vote-event\">Vote event</a>'),
(263, '<h3>Vote Event</h3><br/><br/>
Zapraszamy do głosowania na serwer Hellground na toplistach gier. Na każdej liście można głosować co <b>12</b> godzin. <br/>
Zapoznaj się również z zasadami eventu - <a href=\"http://wow.hellground.pl/zasady-voteevent/\">http://wow.hellground.pl/zasady-voteevent/</a>.<br/>
Aby punkty zostały doliczone musisz kliknąć lewym przyciskiem myszy.<br/><br/>
Listy:', '<h3>Vote Event</h3><br/><br/>
We invite you to vote on Hellground Server on game toplists. You can vote on every list every <b>12</b> hours. <br/>
Also, please read vote event rules - <a href=\"http://wowen.hellground.pl/rules-voteevent/\">http://wowen.hellground.pl/rules-voteevent/</a>.<br/>
You must use left mouse button if you want to have added vote points.<br/><br/>
Toplists:'),
(264, ' Następne głosowanie: ', ' Next vote time: '),
(265, 'Zagłosowano pomyślnie', 'Vote successfull'),
(266, 'Domyślne serwerowe.', 'Server default.'),
(267, 'Blizzlike', 'Blizzlike'),
(268, 'Character page info TODO', 'Character page info TODO'),
(269, '%u dni %u godzin %u minut', '%u days %u hours %u minutes'),
(270, 'Tak', 'Yes'),
(271, 'Nie', 'No'),
(272, 'Postać została odzyskana', 'Character restored'),

(350, 'Błędny login lub hasło', 'Wrong login or password'),
(351, 'Błędny login lub email', 'Wrong login or email'),
(352, 'Podane hasła muszą być takie same', 'Typed passwords must be the same'),
(353, 'Podane hasło jest zbyt krótkie', 'Typed password is too short'),
(354, 'Podane hasło jest zbyt długie', 'Typed password is too long'),
(355, 'Błąd wprowadzonych danych.', 'Wrong input data.'),
(356, 'Postać musi być wylogowana, aby użyć teleportacji.', 'Character must be OFFLINE to use teleport option.'),
(357, 'Postać nie została znaleziona !', 'Character not found !'),
(358, 'Konto posiada włączoną blokadę IP oraz Twój aktualny IP nie pasuje do IP konta.<br/><br/>

Jeśli to konto jest Twoje użyj opcji odzyskiwania hasła w celu zresetowania hasła i blokady IP.', 'Account where you tried to login has IP lock on and your current IP doesn''t match to account IP.<br/><br/>

If it''s your account please use password recovery option to reset password and IP lock.'),
(359, 'Błędne hasło.', 'Wrong password.'),
(360, 'Nie możesz jeszcze zagłosować na tą listę.', 'You can''t vote on that list yet.'),
(361, 'Ta strona wymaga włączonego java script.', 'Java script is required for this site.'),
(362, 'Postać o takim imieniu już istnieje.', 'Character with that name already exists.'),
(363, 'Nie możesz posiadać na koncie postaci z różnych frakcji.', 'You can''t have characters from different factions on one account.'),
(364, 'Zbyt duża ilość postaci na koncie.', 'To much characters on account.'),

(501, 'DB Error: Błąd połączenia z bazą danych.', 'DB Error: Can''t connect to database.'),
(502, 'DB Error: Zapytanie zwróciło pusty wynik.', 'DB Error: Query result was empty.'),
(503, 'DB Error: Wystąpił błąd w zapytaniu.', 'DB Error: Database query error.'),

(601, 'Próba zalogowania na konto zakończona powodzeniem.', 'Successfull account login.'),
(602, 'Próba zalogowania na konto zakończona niepowodzeniem.', 'Unsuccessful account login.'),
(603, 'Konto zostało zarejestrowane.', 'Successfull account registration.'),
(604, 'Próba zmiany hasła.', 'Someone tried to change password.'),
(605, 'Próba odzyskania hasła zakończona powodzeniem.', 'Successfull password recovery.'),
(606, 'Próba odzyskania hasła zakończona niepowodzeniem.', 'Unsuccessful password recovery.'),
(607, 'Próba zmiany ustawień ip locka.', 'Someone tried to change ip lock state.'),
(608, 'Próba zmiany ustawień XP rate.', 'Someone tried to change XP rates.'),
(609, 'Postać %s została odzyskana.', 'Character %s restored.'),

(701, 'ID zadania', 'Quest ID'),
(702, 'Nazwa zadania', 'Quest name'),
(703, 'Poziom zadania', 'Quest level'),
(704, 'Status wykonania', 'Quest status'),

(711, 'ID czaru', 'Spell ID'),
(712, 'Nazwa czaru', 'Spell name'),
(713, 'Aktywny', 'Active'),
(714, 'Wyłączony', 'Disabled'),

(721, 'ID przedmiotu', 'Item ID'),
(722, 'Nazwa przedmiotu', 'Item name'),
(723, 'Ilość', 'Count'),

(726, 'Imię znajomego', 'Friend name'),
(727, 'Notka znajomego', 'Friend note'),

(801, 'Człowiek', 'Human'),
(802, 'Ork', 'Orc'),
(803, 'Krasnolud', 'Dwarf'),
(804, 'Nocny elf', 'Night elf'),
(805, 'Nieumarły', 'Undead'),
(806, 'Tauren', 'Tauren'),
(807, 'Gnom', 'Gnome'),
(808, 'Trol', 'Troll'),
(809, 'Krwawy elf', 'Blood elf'),
(810, 'Draenei', 'Draenei'),

(820, 'Nieznana', 'Unknown'),

(821, 'Wojownik', 'Warrior'),
(822, 'Paladyn', 'Paladin'),
(823, 'Łowca', 'Hunter'),
(824, 'Łotrzyk', 'Rogue'),
(825, 'Ksiądz', 'Priest'),
(826, 'Szaman', 'Shaman'),
(827, 'Mag', 'Mage'),
(828, 'Czarnoksieżnik', 'Warlock'),
(829, 'Druid', 'Druid'),

(851, 'Brak', 'None'),
(852, 'Zadanie ukończone', 'Quest complete'),
(853, 'Zadanie niedostępne', 'Quest unavailable'),
(854, 'Zadanie w trakcie', 'Quest in progress'),
(855, 'Zadanie dostępne', 'Quest available'),
(856, 'Zadanie nagrodzone', 'Quest rewarded');

CREATE TABLE IF NOT EXISTS Activity
(
    id bigint unsigned NOT NULL PRIMARY KEY AUTO_INCREMENT,
    accId bigint(20) unsigned NOT NULL,
    eventDate timestamp NOT NULL,
    ip varchar(30) NOT NULL,
    textId int unsigned NOT NULL DEFAULT 0,
    text TEXT NOT NULL DEFAULT ''
) ENGINE = INNODB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS Vote
(
    id bigint unsigned NOT NULL PRIMARY KEY AUTO_INCREMENT,
    url TEXT NOT NULL,
    imgurl TEXT NOT NULL,
    alttext TEXT NOT NULL DEFAULT '',
    name varchar(30) NOT NULL
) ENGINE = INNODB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS AccVote
(
    acc bigint(20) unsigned NOT NULL,
    voteId bigint unsigned NOT NULL,
    resetDate timestamp NOT NULL,
    PRIMARY KEY (acc, voteId),
    FOREIGN KEY (voteId) REFERENCES Vote(id) ON DELETE CASCADE
) ENGINE = INNODB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS IPVote
(
    ip varchar(15) NOT NULL,
    voteId bigint unsigned NOT NULL,
    resetDate timestamp NOT NULL,
    PRIMARY KEY (ip, voteId),
    FOREIGN KEY (voteId) REFERENCES Vote(id) ON DELETE CASCADE
) ENGINE = INNODB, CHARACTER SET utf8;

DELETE FROM Vote;
INSERT INTO Vote VALUES
('1', 'http://www.xtremetop100.com/in.php?site=1132311845', 'http://www.xtremeTop100.com/votenew.jpg', 'XtremeTop100', 'XtremeTop100'),
('2', 'http://www.wowstatus.net/in.php?server=485129', 'http://www.wowstatus.net/includes/images/vote.gif', 'WoWStatus.net', 'WoWStatus.net'),
('3', 'http://wow.top100arena.com/in.asp?id=62921', 'http://www.top100arena.com/hit.asp?id=62921&c=WoW&t=3', 'wow private server', 'top100arena'),
('4', 'http://www.gamesites200.com/wowprivate/in.php?id=10245', 'http://www.gamesites200.com/wowprivate/vote.gif', 'Vote on the World of Warcraft Private Servers Top 200', 'Game Sites 200'),
('5', 'http://www.mmorpgtoplist.com/in.php?site=43735', 'http://www.mmorpgtoplist.com/vote.jpg', 'World of Warcraft Server', 'mmorpgtoplist'),
('6', 'http://www.gtop100.com/in.php?site=66370', 'http://www.gtop100.com/images/votebutton.jpg', 'Top 100 World of Warcraft sites', 'gtop100'),
('7', 'http://rpgtextowe.topka.pl/?we=nasgothhg', 'http://www.top-rpg.yoyo.pl/152x52ver1.png', 'Toplista-Gier', 'top-rpg'),
('8', 'http://mmorpg.toplista.pl/?we=nasgothhg', 'http://s207272909.onlinehome.us/tm.gif', 'MMORPG Top50', 'MMORPG Top50'),
('9', 'http://www.rpg-paradize.com/?page=vote&vote=30150', 'http://www.rpg-paradize.com/pubmini.gif', 'rpg-paradize', 'rpg-paradize');
