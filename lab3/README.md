# Lab 3 — File Management System

An in-memory file management system: directories, files of several kinds,
users with access rights, versioning, quotas, trash bin, search with
filters and a full operation journal.

Work in progress: the model is being delivered package by package.

## Packages

| Package | Purpose |
|---|---|
| `error` | Domain exception hierarchy |
| `value` | Immutable value objects (paths, sizes, permissions, timestamps, checksums) |
| `entry` | File system entries: directories, files, links |
| `metadata` | Tags and entry version history |
| `user` | Users, groups and access control |
| `storage` | Storage backend, quotas, trash bin, backups |
| `search` | Search filters and the search service |
| `operations` | Operation records and the audit journal |
| `service` | FileManager facade and reports |

Detailed class reference will be added when the model is complete.
