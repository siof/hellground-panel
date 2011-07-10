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
(115, "Informacje o koncie: ", "Account informations: "),
(116, "Założona", "Locked"),
(117, "Zdjęta", "Unlocked"),
(118, "IP jest zbanowane: ", "IP is banned: "),
(130, "Zmiana hasła", "Change password"),
(131, "Stare hasło", "Old pass"),
(132, "Nowe hasło", "New pass"),
(133, "Powtórz hasło", "Repeat pass"),
(134, "Odzyskiwanie hasła", "Regain password"),
(135, "Email", "Email"),
(136, "Numer GG", "GG number"),
(150, "Otwarta", "Opened"),
(151, "Zamknięta", "Closed"),
(155, "Zakładając konto w pełni akceptujesz <a href=\"http://wow.gamefreedom.pl/regulamin-serwera/\">regulamin serwera<\a>", "You read <a href=\"http://gamefreedom.pl/showthread.php?t=8454\">Server Rules<\a> and you accept them?"),
(156, "Tak", "Yes"),
(157, "HellGround - Rejestracja", "HellGround - Register"),
(160, "Login", "Login"),
(161, "Zbanowany od", "Banned from"),
(162, "Zbanowany do", "Banned till"),
(163, "Zbanowany przez", "Banned by"),
(164, "Powód", "Reason"),
(165, "Aktywny", "Active"),
(166, "Nieaktywny", "Not active"),
(167, "Tak", "Yes"),
(168, "Nie", "No"),
(200, "Zmień", "Change"),
(201, "Wyczyść", "Clear"),
(202, "Wyślij", "Send"),
(203, "Zaloguj", "Log in"),
(204, "Zarejestruj", "Register"),
(205, "Teleportuj", "Teleport"),
(206, "Zbanowane konta", "Accounts banned"),
(207, "Zbanowane IP", "IP banned"),
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
(238, "Online", "Online"),
(239, "Offline", "Offline"),
(240, "Aktualne IP jest zbanowane: ", "Current IP is banned: ");
