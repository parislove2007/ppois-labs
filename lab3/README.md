# Lab 3 — File Management System

An in-memory file management system: a tree of directories with files of several
kinds, symbolic links, users with roles and access rights, tags and file versioning,
disk quotas, a trash bin with restore, backup snapshots, flexible search with
composable filters, public share links and a complete audit journal of operations.

## Domain model metrics

| Metric | Value | Required |
| --- | --- | --- |
| Classes (without exceptions) | 57 | >= 50 |
| Exception classes | 14 | >= 12 |
| Fields | 153 | >= 150 |
| Meaningful methods | 231 | >= 100 |
| Associations | 131 | >= 30 |

Recount: `python3 lab3/tools/metrics.py`.

## Class reference

### Value objects

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `Checksum` | `value_` | `calculate`, `verify`, `operator==` | — |
| `Clock` | `current_`, `stepSeconds_` | `next`, `now` | `Timestamp` |
| `FileExtension` | `value_` | `fromFileName`, `isEmpty`, `isText`, `isImage`, `isArchive`, `isKnown`, `operator==` | — |
| `FileSize` | `bytes_` | `fromBytes`, `fromKilobytes`, `isZero`, `fitsIn`, `operator+`, `operator-`, `operator<`, `operator==` | — |
| `Path` | `value_` | `parse`, `toString`, `name`, `parent`, `extension`, `isAbsolute`, `segments`, `join`, `operator==` | — |
| `Permission` | `read_`, `write_`, `execute_` | `none`, `readOnly`, `readWrite`, `full`, `canRead`, `canWrite`, `canExecute`, `withRead`, `withWrite`, `withExecute`, `allows`, `toString`, `operator==` | — |
| `Timestamp` | `seconds_` | `of`, `plusSeconds`, `diffSeconds`, `isBefore`, `isAfter`, `operator==` | — |

### Exceptions

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `ChecksumMismatchException` | — | — | — |
| `EmptyTrashException` | — | — | — |
| `EntryAlreadyExistsException` | — | — | — |
| `FileLockedException` | — | — | — |
| `FileSystemException` | — | — | — |
| `InvalidFileNameException` | — | — | — |
| `InvalidFileSizeException` | — | — | — |
| `NotADirectoryException` | — | — | — |
| `NotAFileException` | — | — | — |
| `PathNotFoundException` | — | — | — |
| `PermissionDeniedException` | — | — | — |
| `QuotaExceededException` | — | — | — |
| `StorageFullException` | — | — | — |
| `VersionConflictException` | — | — | — |

### File system entries

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `ArchiveFile` | `packedNames_`, `compressionLevel_`, `comment_` | `kind`, `packEntry`, `packedCount`, `containsPacked` | `Permission`, `Timestamp` |
| `BinaryFile` | `mimeType_` | `kind`, `append` | `Permission`, `Timestamp` |
| `Directory` | `entries_`, `entryLimit_` | `add`, `remove`, `find`, `contains`, `entryCount`, `size`, `describe`, `isDirectory` | `FileSize`, `FileSystemEntry`, `Permission`, `Timestamp` |
| `File` | `content_`, `checksum_` | `kind`, `write`, `read`, `size`, `describe` | `Checksum`, `FileSize`, `Permission`, `Timestamp` |
| `FileSystemEntry` | `name_`, `owner_`, `permission_`, `created_`, `modified_` | `size`, `describe`, `isDirectory`, `rename`, `changePermission`, `touch` | `FileSize`, `Permission`, `Timestamp` |
| `SymbolicLink` | `target_`, `followCount_` | `retarget`, `registerFollow`, `size`, `describe` | `FileSize`, `Path`, `Permission`, `Timestamp` |
| `TextFile` | `encoding_`, `lineEnding_` | `kind`, `lineCount`, `wordCount` | `Permission`, `Timestamp` |

### Metadata and versioning

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `EntryVersion` | `number_`, `content_`, `saved_`, `author_`, `comment_` | `checksum` | `Checksum`, `Timestamp` |
| `FileMetadata` | `description_`, `tags_`, `author_`, `language_`, `createdBy_`, `isFavorite_`, `rating_` | `addTag`, `removeTag`, `hasTag`, `tagCount`, `toggleFavorite` | `Tag` |
| `Tag` | `label_`, `category_`, `color_` | `operator==` | — |
| `VersionHistory` | `versions_`, `maxVersions_` | `addVersion`, `count`, `latest`, `findByNumber`, `rollbackTo`, `trimToLimit` | `EntryVersion`, `Timestamp` |

### Users and access control

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `AccessControlList` | `rules_`, `defaultPermission_` | `grant`, `revoke`, `hasRuleFor`, `permissionFor`, `allows` | `Permission` |
| `Administrator` | — | `role`, `defaultPermission`, `canBypassQuota` | `Permission`, `User` |
| `Group` | `name_`, `ownerLogin_`, `maxMembers_`, `members_` | `isOwnedBy`, `addMember`, `removeMember`, `hasMember`, `memberCount` | — |
| `GroupRegistry` | `groups_` | `createGroup`, `findByName`, `names` | `Group` |
| `Guest` | — | `role`, `defaultPermission` | `Permission`, `User` |
| `Lock` | `path_`, `owner_`, `lockedAt_`, `comment_` | `isStale` | `Path`, `Timestamp` |
| `LockManager` | `locks_` | `lock`, `unlock`, `isLocked`, `ownerOf`, `releaseStale` | `Lock`, `Path`, `Timestamp` |
| `RegularUser` | — | `role`, `defaultPermission` | `Permission`, `User` |
| `User` | `login_`, `displayName_`, `registered_`, `email_`, `department_`, `isActive_`, `lastLoginAt_` | `deactivate`, `activate`, `role`, `defaultPermission`, `canBypassQuota`, `markLoggedIn` | `Permission`, `Timestamp`, `UserRegistry` |
| `UserRegistry` | `users_`, `credentials_`, `maxFailedAttempts_` | `registerUser`, `findByLogin`, `authenticate`, `logins` | `Timestamp`, `User` |

### Storage, quotas and backups

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `BackupRecord` | `path_`, `content_`, `checksum_` | `verify` | `Checksum`, `Path` |
| `BackupService` | `records_`, `label_`, `createdAt_`, `isEncrypted_` | `capture`, `capture`, `toggleEncryption`, `describe`, `recordCount`, `has`, `contentOf`, `restoreTo` | `BackupRecord`, `Path`, `Storage`, `Timestamp` |
| `Quota` | `login_`, `limit_`, `used_` | `remaining`, `charge`, `release` | `FileSize` |
| `QuotaManager` | `defaultQuota_`, `quotas_`, `used_` | `quotaOf`, `usedOf`, `accountWritten`, `accountFreed` | `FileSize` |
| `Storage` | `root_`, `capacity_`, `metadata_` | `resolve`, `resolve`, `ensureDirectory`, `attach`, `detach`, `metadataFor`, `hasMetadata`, `findMetadata`, `walk`, `ensureCapacity`, `parentDirectoryFor` | `Directory`, `FileMetadata`, `FileSize`, `FileSystemEntry`, `Path`, `Timestamp` |
| `TrashBin` | `items_`, `maxItems_`, `restoredCount_`, `clearedCount_` | `moveToTrash`, `itemCount`, `entryAt`, `originAt`, `restore`, `clear`, `checkIndex` | `FileSystemEntry`, `Path` |

### Search

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `CompositeFilter` | `matchAll_`, `filters_` | `add`, `matches` | `FileSystemEntry`, `Path`, `SearchFilter` |
| `DateFilter` | `from_`, `to_`, `useCreated_` | `matches` | `FileSystemEntry`, `Path`, `Timestamp` |
| `ExtensionFilter` | `extension_` | `matches` | `FileExtension`, `FileSystemEntry`, `Path` |
| `NameFilter` | `needle_`, `caseSensitive_` | `matches` | `FileSystemEntry`, `Path` |
| `SearchFilter` | — | `matches` | `FileSystemEntry`, `Path` |
| `SearchService` | — | `search` | `Path`, `SearchFilter`, `Storage` |
| `SizeFilter` | `min_`, `max_` | `matches` | `FileSize`, `FileSystemEntry`, `Path` |
| `TagFilter` | `storage_`, `label_` | `matches` | `FileSystemEntry`, `Path`, `Storage` |

### Operations and audit

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `CopyOperation` | `from_`, `to_` | `kind`, `describe` | `Path`, `Timestamp` |
| `CreateOperation` | `path_` | `kind`, `describe` | `Path`, `Timestamp` |
| `DeleteOperation` | `path_` | `kind`, `describe` | `Path`, `Timestamp` |
| `MoveOperation` | `from_`, `to_` | `kind`, `describe` | `Path`, `Timestamp` |
| `Operation` | `at_` | `kind`, `describe` | `Timestamp` |
| `OperationJournal` | `entries_`, `maxEntries_` | `log`, `count`, `at`, `byKind`, `historyText` | `Operation` |
| `RenameOperation` | `path_`, `newName_` | `kind`, `describe` | `Path`, `Timestamp` |

### Services

| Class | Fields | Methods | Associated classes |
| --- | --- | --- | --- |
| `DirectoryListing` | `path_`, `entryNames_`, `fileCount_`, `directoryCount_`, `totalBytes_` | `toLine` | `FileSize`, `Path` |
| `FileInfo` | `path_`, `owner_`, `kind_`, `size_`, `isDirectory_`, `modified_`, `permission_` | `toLine` | `FileSize`, `Path`, `Permission`, `Timestamp` |
| `FileManager` | `storage_`, `quotas_`, `trash_`, `journal_`, `backup_`, `histories_` | `createDirectory`, `write`, `saveVersion`, `versionHistoryFor`, `rollback`, `remove`, `restoreFromTrash`, `move`, `copyFile`, `rename`, `tag`, `describe`, `search`, `makeBackup`, `restoreBackup` | `BackupService`, `FileSize`, `OperationJournal`, `Path`, `QuotaManager`, `SearchFilter`, `Storage`, `Tag`, `Timestamp`, `TrashBin`, `User`, `VersionHistory` |
| `ReportService` | — | `treeText`, `summaryLine`, `computeStats`, `collectInfo`, `listDirectory` | `DirectoryListing`, `FileInfo`, `FileSize`, `Path`, `Storage`, `StorageStats` |
| `SearchQuery` | `needle_`, `caseSensitive_`, `extension_`, `minSize_`, `maxSize_`, `modifiedAfter_`, `tag_`, `hasName_`, `hasExtension_`, `hasSize_`, `hasTime_`, `hasTag_` | `withName`, `withExtension`, `withSizeRange`, `withModifiedAfter`, `withTag`, `isEmpty`, `buildFilter` | `FileExtension`, `FileSize`, `SearchFilter`, `Storage`, `Timestamp` |
| `ShareLink` | `token_`, `path_`, `owner_`, `createdAt_`, `expiresAt_`, `hits_`, `revoked_` | `isExpired`, `registerHit`, `revoke`, `isRevoked` | `Path`, `Timestamp` |
| `ShareService` | `links_`, `nextId_` | `resolve`, `revoke`, `activeCount` | `Path`, `ShareLink`, `Timestamp` |
| `StorageStats` | `directories_`, `files_`, `links_`, `totalBytes_`, `computedAt_` | `countDirectory`, `countFile`, `countLink`, `addBytes`, `toLine`, `stamp` | `FileSize`, `Timestamp` |

## Building and testing

```bash
cmake -S lab3 -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Coverage build (required line coverage is 90%+):

```bash
cmake -S lab3 -B build-cov -DENABLE_COVERAGE=ON
cmake --build build-cov
ctest --test-dir build-cov
gcovr --root . --filter 'lab3/src/' --fail-under-line 90 build-cov
```

## Console interface

The console app exposes the facade: user switching, directories and files,
write and read, trash with restore, versioning with rollback, search,
tags, quotas, backups, share links and the audit journal.

```bash
./build/lab3_app
> user ivan
> mkdir /docs
> touch /docs/hello.txt hello world
> read /docs/hello.txt
> share /docs/hello.txt
> journal
> exit
```

## Design notes

- `FileManager` is the only facade: the console and reports know nothing
  about the internals of the domain packages.
- Four polymorphic hierarchies: file system entries, users, search filters
  and audit operations.
- Directories own entries through `unique_ptr`; the trash bin and backup
  service reuse the same ownership model.
- Rollback appends the old content as a new version instead of rewriting
  history, mirroring version control systems.
- `ShareLink::registerHit` is a `const` method with a `mutable` counter:
  resolving a link does not change its observable state.

