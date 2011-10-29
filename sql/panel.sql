CREATE TABLE IF NOT EXISTS LangTexts
(
    id integer NOT NULL PRIMARY KEY,
    lang_0 TEXT NOT NULL,
    lang_1 TEXT NOT NULL
) ENGINE = INNODB, CHARACTER SET utf8;

DELETE FROM LangTexts;

INSERT INTO LangTexts VALUES
(0, "Strona główna", "Home"),
(1, "Zaloguj", "Log in"),
(2, "Wyloguj", "Log out"),
(3, "Zarejestruj", "Register"),
(4, "Odzyskaj hasło", "Restore pass"),
(5, "Zmień hasło", "Pass change"),
(6, "Informacje o koncie", "Account informations"),
(7, "Status serwera", "Server status"),
(8, "Dotacja", "Donate"),
(9, "Vote event", "Vote event"),
(10, "Linki", "Links"),
(11, "Teleport", "Teleport"),
(12, "Blokada IP", "IP lock"),
(13, "Informacje o banach", "Ban info"),
(14, "Lista banów", "Ban list"),
(15, "Informacje o mute", "Mute info"),
(16, "Lista mute", "Mute list"),
(17, "ERROR", "ERROR"),
(18, "Licencja", "Licence"),
(19, "Support", "Support"),

(50, "Panel GM", "GM Panel"),
(51, "Tickety", "Tickets"),
(52, "Sprawdź online", "Online check"),
(53, "Sprawdź frakcje", "Faction check"),

(75, "Nick: ", "Nick: "),
(76, "Poziom: ", "Level: "),
(77, "Klasa: ", "Class: "),
(78, "Całkowity czas gry: ", "Total played time: "),
(79, "Czas gry aktualnego poziomu: ", "This level played time: "),

(100, "Postacie na koncie: ", "Characters on account: "),
(101, "Data założenia konta: ", "Account create date: "),
(102, "Ostatnie logowanie: ", "Last login: "),
(103, "Ostatnie IP: ", "Last IP: "),
(104, "Blokada IP: ", "IP lock: "),
(105, "Zbanowane: ", "Banned: "),
(106, "Online: ", "Online: "),
(107, "Vote Points: ", "Vote Points: "),
(108, "Dodatek: ", "Expansion: "),
(109, "Wersja klienta: ", "Client version: "),
(110, "Multikonto: ", "Multiaccount: "),
(111, "Ostatnie odzyskiwanie hasła: ", "Last pass restore: "),
(112, "Login do konta: ", "Account login: "),
(113, "Email: ", "Email: "),
(114, "Numer GG: ", "GG number: "),
(115, "<h3>Informacje o koncie:</h3>", "<h3>Account informations:</h3>"),
(116, "Założona (zdejmij)", "Locked (unlock)"),
(117, "Zdjęta (załóż)", "Unlocked (lock)"),
(118, "IP jest zbanowane: ", "IP is banned: "),
(119, "Ogólne Informacje", "Basic Informations"),
(120, "Bany", "Bans"),
(121, "Mute", "Mute"),
(122, "Tickety", "Tickets"),
(123, "Historia aktywności", "Activity history"),

(130, "Zmiana hasła", "Change password"),
(131, "Stare hasło", "Old pass"),
(132, "Nowe hasło", "New pass"),
(133, "Powtórz hasło", "Repeat pass"),
(134, "Odzyskiwanie hasła", "Regain password"),
(135, "Email", "Email"),
(136, "Numer GG", "GG number"),

(150, "Otwarta", "Opened"),
(151, "Zamknięta", "Closed"),

(155, "Zakładając konto w pełni akceptujesz <a href=\"http://wow.gamefreedom.pl/regulamin-serwera/\">regulamin serwera</a>", "You read <a href=\"http://gamefreedom.pl/showthread.php?t=8454\">Server Rules</a> and you accept them?"),
(156, "Tak", "Yes"),
(157, "<h3>HellGround - Rejestracja</h3>", "<h3>HellGround - Register</h3>"),
(158, "Aby założyć konto musisz zaakceptować regulamin serwera.", "You must accept server rules to register new account."),
(159, "Konto istnieje. Prosze wybrać inny login.", "Account already exists. Please try another login."),

(160, "Login", "Login"),
(161, "Zbanowany od", "Banned from"),
(162, "Zbanowany do", "Banned till"),
(163, "Zbanowany przez", "Banned by"),
(164, "Powód", "Reason"),
(165, "Aktywny", "Active"),
(166, "Nieaktywny", "Not active"),
(167, "Tak", "Yes"),
(168, "Nie", "No"),

(170, "Zmutowany od", "Muted from"),
(171, "Zmutowany do", "Muted till"),
(172, "Zmutowany przez", "Muted by"),
(173, "Powód", "Reason"),
(174, "Aktywny", "Active"),
(175, "Nieaktywny", "Not active"),
(176, "Tak", "Yes"),
(177, "Nie", "No"),

(185, "Nazwa realmu: ", "Realm name: "),
(186, "Stan realmu: ", "Realm state: "),
(187, "Graczy online: ", "Players online: "),
(188, "Najwięcej online: ", "Max players online: "),
(189, "Horda: %i % Przymierze: %i %", "Horde: %i % Alliance: %i %"),
(190, "Realm uptime: ", "Realm uptime: "),
(191, "Rev: ", "Rev: "),
(192, "Diff: ", "Diff: "),
(193, "Średni diff: ", "Avarage diff: "),
(194, "Jak zagrać ? <a href=\"http://wow.gamefreedom.pl/jak-zagrac\" target=\"_blank\"><font color=\"#ffdf81\">Opis</font></a>", "Haw to play ? <a href=\"http://en.hellground.pl/how-to-play\" target=\"_blank\"><font color=\"#ffdf81\">Description</font></a>"),
(195, "<h3>Status serwerów</h3>
Aktualizacja co 60 sekund.", "<h3>Servers status</h3>
Refreshes every 60 seconds."),

(200, "Zmień", "Change"),
(201, "Wyczyść", "Clear"),
(202, "Wyślij", "Send"),
(203, "Zaloguj", "Log in"),
(204, "Zarejestruj", "Register"),
(205, "Teleportuj", "Teleport"),
(206, "Zbanowane konta", "Accounts banned"),
(207, "Zbanowane IP", "IP banned"),
(208, "Wyloguj", "Logout"),

(225, "Panel gracza - HellGround", "HellGround Players Panel"),
(226, "Logo World of Warcraft, is a registered trademark Blizzard Entertainment.
© 2006-2011 Panel created by HellGround.pl ", "Logo World of Warcraft, is a registered trademark Blizzard Entertainment.
© 2006-2011 Panel created by HellGround.pl "),
(227, "Informacje o serwerze TODO", "Server info TODO"),
(228, "Teleport postaci TODO", "Character teleport/unstack TODO"),
(229, "Blokada włączona", "IP Lock ON"),
(230, "Blokada wyłączona", "IP Lock OFF"),
(231, "Bany TODO", "Bans TODO"),
(232, "Autostatus instancji TODO", "Accessible instances TODO"),
(233, "World Of Warcraft", "World Of Warcraft"),
(234, "World Of Warcraft: The Burning Crusade", "World Of Warcraft: The Burning Crusade"),
(235, "World Of Warcraft: Wrath of the Lich King", "World Of Warcraft: Wrath of the Lich King"),
(236, "World Of Warcraft: Cataclysm", "World Of Warcraft: Cataclysm"),
(237, "Aktualne IP: ", "Current IP: "),
(238, "Tak", "Yes"),
(239, "Nie", "No"),
(240, "Aktualne IP jest zbanowane: ", "Current IP is banned: "),
(241, "Mail rejestracyjny.

Login: %s pass: %s", "Registration mail.

 Login: %s pass: %s"),
(242, "HellGround Server - Rejestracja konta", "Hellground Server - Account registration"),
(243, "Mail odzyskiwania hasła.

Data odzyskiwania: %s
IP sesji odzyskiwania: %s
Nowe hasło: %s", "Password Recovery Mail.

Recovery date: %s
Recovery session IP: %s
New pass: %s"),
(244, "HellGround Server - Odzyskiwanie hasła", "Hellground Server - Password recovery"),
(245, "Rejstracja zakończona sukcesem. Hasło zostało wysłane na podany adres email.", "Registration complete. Password was sent to given email address."),
(246, "Przywracanie hasła zakończone sukcesem. Nowe hasło zostało wysłane na email.", "Password recovery complete. New password was sent on account email address."),
(247, "Zmiana hasła zakończona sukcesem.", "Password change successfull."),
(248, "Online", "Online"),
(249, "Offline", "Offline"),
(250, "%i dni %i godzin %i minut %i sekund", "%i days %i hours %i minuts %i seconds"),
(251, "Na pewno chcesz sie wylogować ?", "Are you sure you want to logout ?"),
(252, "Wystąpił błąd podczas rejestracji. Spróbuj ponownie.", "There was an error in registration. Try again."),
(253, "Teleportacja postaci zakończona powodzeniem.", "Character teleportation successfull."),
(254, "Twoje konto nie zostało jeszcze zbanowane.", "Your account was never banned."),
(255, "Twoje konto nie zostało jeszcze zmutowane.", "Your account was never muted."),
(256, "Data", "Date"),
(257, "Adres IP", "IP address"),
(258, "Aktywność", "Activity"),
(259, "HG Players Panel - webowy panel dla graczy serwera HellGround
Copyright (C) 2011 HellGround Team : Siof, lukaasm,

Ten program stanowi wolne oprogramowanie: możesz je rozpowszechniać
i/lub modyfikować na warunkach GNU Affero Powszechnej Licencji
Publicznej wydanej przez Free Software Foundation w wersji 3.

Ten program jest rozpowszechniany z nadzieją, że jest przydatny, ale
BEZ JAKIEJKOLWIEK GWARANCJI, nawet bez domyślnej gwarancji
PRZYDATNOŚCI HANDLOWEJ ORAZ PRZYDATNOŚCI DO OKREŚLONYCH ZASTOSOWAŃ.
Przeczytaj GNU Affero Powszechną Licencję Publiczną w celu zapoznania
się ze szczegółami.

Powinieneś otrzymać kopię GNU Affero Powszechnej Licencji Publiczej
wraz z programem. Jeżeli nie otrzymałeś jej, zobacz na
< http://www.gnu.org/licenses/ >

kontakt: siof@hellground.pl", "HG Players Panel - web panel for HellGround server Players
Copyright (C) 2011 HellGround Team : Siof, lukaasm,

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License version 3 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see < http://www.gnu.org/licenses/ >.

contact: siof@hellground.pl"),
(260, "Repozytorium kodu źródłowego (zawiera także licencję): <a href=\"https://bitbucket.org/siof/hgplayerspanel\">https://bitbucket.org/siof/hgplayerspanel</a>", "Source code repository (also contains licence): <a href=\"https://bitbucket.org/siof/hgplayerspanel\">https://bitbucket.org/siof/hgplayerspanel</a>"),
(261, "", ""),
(262, "Support info TODO", "Support info TODO"),
(263, "Vote info TODO", "Vote info TODO"),
(264, " Następne głosowanie: ", " Next vote time: "),
(265, "Zagłosowano pomyślnie", "Vote successfull"),

(350, "Błędny login lub hasło", "Wrong login or password"),
(351, "Błędny login lub email", "Wrong login or email"),
(352, "Podane hasła muszą być takie same", "Typed passwords must be the same"),
(353, "Podane hasło jest zbyt krótkie", "Typed password is to short"),
(354, "Podane hasło jest zbyt długie", "Typed password is to long"),
(355, "Błąd validacji! Podano niepoprawne dane.", "Validation error! Wrong input data."),
(356, "Postać musi być OFFLINE aby móc użyć opcji teleportacji.", "Character must be OFFLINE to use teleport option."),
(357, "Postać nie została znaleziona !", "Character not found !"),
(358, "Konto posiada włączoną blokadę IP oraz Twój aktualny IP nie pasuje do IP konta.

Jeśli to konto jest Twoje użyj opcji odzyskiwania hasła w celu zresetowania hasła i blokady IP.", "Account where you tried to login has IP lock on and your current IP doesn't match to account IP.

If it's your account please use password recovery option to reset password and IP lock."),
(359, "Błędne hasło.", "Wrong password."),
(360, "Nie możesz jeszcze zagłosować na tą listę.", "You can't vote on that list yet."),
(361, "Ta strona wymaga włączonego java script.", "Java script is required for this site."),

(501, "DB Error: Błąd połączenia z bazą danych.", "DB Error: Can't connect to database."),
(502, "DB Error: Zapytanie zwróciło pusty wynik.", "DB Error: Query result was empty."),
(503, "DB Error: Wystąpił błąd w zapytaniu.", "DB Error: Database query error."),

(601, "Próba zalogowania na konto zakończona powodzeniem.", "Successfull account login."),
(602, "Próba zalogowania na konto zakończona niepowodzeniem.", "Someone tried to log in (unsuccessfull)."),
(603, "Konto zostało zarejestrowane.", "Successfull account registration."),
(604, "Próba zmiany hasła.", "Someone tried to change password."),
(605, "Próba odzyskania hasła zakończona powodzeniem.", "Successfull password recovery."),
(606, "Próba odzyskania hasła zakończona niepowodzeniem.", "Someone tried to recover password (unsuccessfull)."),
(607, "Próba zmiany ustawień ip locka.", "Someone tried to change ip lock state.");

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

DELETE FROM Vote;
INSERT INTO Vote VALUES
('XXX', 'http://www.xtremetop100.com/in.php?site=1132311845', 'http://www.xtremeTop100.com/votenew.jpg', 'XtremeTop100', 'XtremeTop100'),
('XXX', 'http://www.wowstatus.net/in.php?server=485129', 'http://www.wowstatus.net/includes/images/vote.gif', 'WoWStatus.net', 'WoWStatus.net');
