#include <gtest/gtest.h>

#include <vector>

#include "error/EntryAlreadyExistsException.h"
#include "error/PathNotFoundException.h"
#include "user/AccessControlList.h"
#include "user/Administrator.h"
#include "user/Group.h"
#include "user/Guest.h"
#include "user/RegularUser.h"
#include "user/User.h"
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
