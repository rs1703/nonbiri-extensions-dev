CREATE TABLE IF NOT EXISTS extension (
  id TEXT NOT NULL PRIMARY KEY,
  domain TEXT NOT NULL,
  obsolete INTEGER NOT NULL DEFAULT 0
);

CREATE UNIQUE INDEX IF NOT EXISTS extension_uidx ON extension(domain);
CREATE INDEX IF NOT EXISTS extension_obsolete_idx ON extension(obsolete);