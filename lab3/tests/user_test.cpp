#include <gtest/gtest.h>

#include <vector>

#include <memory>

#include "error/EntryAlreadyExistsException.h"
#include "error/FileLockedException.h"
#include "error/FileSystemException.h"
#include "error/PathNotFoundException.h"
#include "error/PermissionDeniedException.h"
#include "user/AccessControlList.h"
#include "user/Administrator.h"
#include "user/Group.h"
#include "user/GroupRegistry.h"
#include "user/Guest.h"
#include "user/Lock.h"
#include "user/LockManager.h"
#include "user/RegularUser.h"
#include "user/User.h"
#include "user/UserRegistry.h"
#include "value/Permission.h"
#include "value/Timestamp.h"

using namespace fs;

TEST(UserRolesTest, RolesDefinePermissions)
{
    Administrator admin("root", "Root", Timestamp::of(1));
    RegularUser user("ivan", "Ivan", Timestamp::of(2));
    Guest guest("guest", "Guest", Timestamp::of(3));

    EXPECT_EQ(admin.login(), "root");
    EXPECT_EQ(admin.role(), "administrator");
    EXPECT_TRUE(admin.defaultPermission() == Permission::full());
    EXPECT_TRUE(admin.canBypassQuota());

    EXPECT_EQ(user.role(), "regular");
    EXPECT_TRUE(user.defaultPermission() == Permission::readWrite());
    EXPECT_FALSE(user.canBypassQuota());

    EXPECT_EQ(guest.role(), "guest");
    EXPECT_TRUE(guest.defaultPermission() == Permission::readOnly());
}

TEST(UserRolesTest, WorksPolymorphically)
{
    Administrator admin("root", "Root", Timestamp::of(1));
    RegularUser user("ivan", "Ivan", Timestamp::of(2));
    Guest guest("guest", "Guest", Timestamp::of(3));

    std::vector<User*> users{&admin, &user, &guest};
    EXPECT_EQ(users[0]->role(), "administrator");
    EXPECT_EQ(users[1]->role(), "regular");
    EXPECT_EQ(users[2]->role(), "guest");
}

TEST(GroupTest, TracksMembersByLogin)
{
    Group group("editors");
    EXPECT_EQ(group.name(), "editors");
    group.addMember("ivan");
    group.addMember("anna");
    EXPECT_EQ(group.memberCount(), 2u);
    EXPECT_TRUE(group.hasMember("ivan"));
    EXPECT_THROW(group.addMember("ivan"), EntryAlreadyExistsException);

    group.removeMember("ivan");
    EXPECT_FALSE(group.hasMember("ivan"));
    EXPECT_THROW(group.removeMember("ivan"), PathNotFoundException);
}

TEST(AccessControlListTest, DeniesEverythingWithoutRule)
{
    AccessControlList acl;
    EXPECT_FALSE(acl.hasRuleFor("ivan"));
    EXPECT_FALSE(acl.allows("ivan", Permission::readOnly()));
    EXPECT_THROW(acl.permissionFor("ivan"), PathNotFoundException);
    EXPECT_THROW(acl.revoke("ivan"), PathNotFoundException);
}

TEST(AccessControlListTest, GrantsAndReplacesRules)
{
    AccessControlList acl;
    acl.grant("ivan", Permission::readOnly());
    EXPECT_TRUE(acl.hasRuleFor("ivan"));
    EXPECT_TRUE(acl.allows("ivan", Permission::readOnly()));
    EXPECT_FALSE(acl.allows("ivan", Permission::readWrite()));

    acl.grant("ivan", Permission::readWrite());
    EXPECT_TRUE(acl.allows("ivan", Permission::readWrite()));

    acl.revoke("ivan");
    EXPECT_FALSE(acl.hasRuleFor("ivan"));
}

TEST(UserTest, CarriesProfileAndActivity)
{
    RegularUser user("ivan", "Ivan", Timestamp::of(1), "ivan@example.com", "IT");
    EXPECT_EQ(user.email(), "ivan@example.com");
    EXPECT_EQ(user.department(), "IT");
    EXPECT_TRUE(user.isActive());
    user.deactivate();
    EXPECT_FALSE(user.isActive());
    user.activate();
    EXPECT_TRUE(user.isActive());
    EXPECT_EQ(user.lastLoginAt().seconds(), 0);
}

TEST(LockTest, DetectsStaleLocks)
{
    const Lock lock(Path::parse("/a.txt"), "ivan", Timestamp::of(100), "editing");
    EXPECT_EQ(lock.owner(), "ivan");
    EXPECT_EQ(lock.comment(), "editing");
    EXPECT_FALSE(lock.isStale(Timestamp::of(150), 100));
    EXPECT_TRUE(lock.isStale(Timestamp::of(250), 100));
}

TEST(LockManagerTest, EnforcesExclusiveAccess)
{
    LockManager locks;
    const Path file = Path::parse("/a.txt");
    locks.lock(file, "ivan", Timestamp::of(1));
    EXPECT_TRUE(locks.isLocked(file));
    EXPECT_EQ(locks.ownerOf(file), "ivan");
    EXPECT_THROW(locks.lock(file, "anna", Timestamp::of(2)), FileLockedException);
    EXPECT_NO_THROW(locks.lock(file, "ivan", Timestamp::of(3)));
    EXPECT_THROW(locks.unlock(file, "anna"), FileLockedException);
    locks.unlock(file, "ivan");
    EXPECT_FALSE(locks.isLocked(file));
    EXPECT_THROW(locks.unlock(file, "ivan"), FileLockedException);
}

TEST(LockManagerTest, ReleasesStaleLocks)
{
    LockManager locks;
    locks.lock(Path::parse("/a.txt"), "ivan", Timestamp::of(0));
    locks.lock(Path::parse("/b.txt"), "ivan", Timestamp::of(160));
    EXPECT_EQ(locks.releaseStale(Timestamp::of(200), 50), 1u);
    EXPECT_FALSE(locks.isLocked(Path::parse("/a.txt")));
    EXPECT_TRUE(locks.isLocked(Path::parse("/b.txt")));
}

TEST(UserRegistryTest, RegistersAndAuthenticates)
{
    UserRegistry registry(3);
    registry.registerUser(std::make_unique<RegularUser>("ivan", "Ivan", Timestamp::of(1)),
                          "secret");
    EXPECT_THROW(
        registry.registerUser(std::make_unique<Guest>("ivan", "Ivan", Timestamp::of(2)),
                              "other"),
        EntryAlreadyExistsException);
    EXPECT_NE(registry.findByLogin("ivan"), nullptr);
    EXPECT_EQ(registry.findByLogin("ghost"), nullptr);
    EXPECT_EQ(registry.logins(), (std::vector<std::string>{"ivan"}));

    User& logged = registry.authenticate("ivan", "secret", Timestamp::of(10));
    EXPECT_EQ(logged.login(), "ivan");
    EXPECT_EQ(logged.lastLoginAt().seconds(), 10);
}

TEST(UserRegistryTest, BlocksAfterRepeatedFailures)
{
    UserRegistry registry(3);
    registry.registerUser(std::make_unique<RegularUser>("ivan", "Ivan", Timestamp::of(1)),
                          "secret");
    for (int attempt = 0; attempt < 3; ++attempt)
    {
        EXPECT_THROW(registry.authenticate("ivan", "wrong", Timestamp::of(2)),
                     PermissionDeniedException);
    }
    EXPECT_THROW(registry.authenticate("ivan", "secret", Timestamp::of(3)),
                 PermissionDeniedException);
    EXPECT_THROW(registry.authenticate("ghost", "secret", Timestamp::of(3)),
                 PermissionDeniedException);
}

TEST(UserRegistryTest, RejectsDeactivatedAccounts)
{
    UserRegistry registry(3);
    auto user = std::make_unique<RegularUser>("ivan", "Ivan", Timestamp::of(1));
    User* raw = user.get();
    registry.registerUser(std::move(user), "secret");
    raw->deactivate();
    EXPECT_THROW(registry.authenticate("ivan", "secret", Timestamp::of(2)),
                 PermissionDeniedException);
}

TEST(GroupTest, TracksOwnerAndMemberLimit)
{
    Group group("editors", "ivan", 2);
    EXPECT_TRUE(group.isOwnedBy("ivan"));
    EXPECT_FALSE(group.isOwnedBy("anna"));
    group.addMember("ivan");
    group.addMember("anna");
    EXPECT_THROW(group.addMember("petr"), FileSystemException);
}

TEST(GroupRegistryTest, CreatesAndFindsGroups)
{
    GroupRegistry registry;
    Group& editors = registry.createGroup("editors");
    editors.addMember("ivan");
    EXPECT_THROW(registry.createGroup("editors"), EntryAlreadyExistsException);
    ASSERT_NE(registry.findByName("editors"), nullptr);
    EXPECT_TRUE(registry.findByName("editors")->hasMember("ivan"));
    EXPECT_EQ(registry.findByName("missing"), nullptr);
    EXPECT_EQ(registry.names(), (std::vector<std::string>{"editors"}));
}

TEST(AccessControlListTest, FallsBackToDefaultPermission)
{
    AccessControlList acl;
    acl.setDefaultPermission(Permission::readOnly());
    EXPECT_TRUE(acl.allows("stranger", Permission::readOnly()));
    acl.grant("ivan", Permission::none());
    EXPECT_FALSE(acl.allows("ivan", Permission::readOnly()));
}
