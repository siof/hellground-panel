ALTER TABLE account
    ADD vote int unsigned NOT NULL default 0 AFTER operatingSystem;
