CREATE TABLE IF NOT EXISTS Activity
(
    account_id INT UNSIGNED NOT NULL,
    event_date TIMESTAMP NOT NULL,
    ip CHAR(16) NOT NULL,
    activity_id VARCHAR(50) NOT NULL,
    activity_args VARCHAR(100) NOT NULL DEFAULT '',
    PRIMARY KEY(account_id, event_date)
) ENGINE = InnoDB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS Vote
(
    id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    url VARCHAR(200) NOT NULL,
    img_url VARCHAR(200) NOT NULL,
    alt_text VARCHAR(200) NOT NULL DEFAULT '',
    name VARCHAR(30) NOT NULL
) ENGINE = InnoDB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS AccVote
(
    account_id INT UNSIGNED NOT NULL,
    vote_id INT UNSIGNED NOT NULL,
    reset_date TIMESTAMP NOT NULL,
    PRIMARY KEY (account_id, vote_id),
    FOREIGN KEY (vote_id) REFERENCES Vote(id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
) ENGINE = InnoDB, CHARACTER SET utf8;

CREATE TABLE IF NOT EXISTS IPVote
(
    ip CHAR(16) NOT NULL,
    vote_id INT UNSIGNED NOT NULL,
    reset_date TIMESTAMP NOT NULL,
    PRIMARY KEY (ip, vote_id),
    FOREIGN KEY (vote_id) REFERENCES Vote(id)
        ON DELETE CASCADE
        ON UPDATE CASCADE
) ENGINE = InnoDB, CHARACTER SET utf8;

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
