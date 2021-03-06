// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/common/metrics/variations/variations_util.h"

#include <set>
#include <string>
#include <vector>

#include "base/metrics/field_trial.h"
#include "components/variations/metrics_util.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace chrome_variations {

class VariationsUtilTest : public ::testing::Test {
 public:
  VariationsUtilTest() : field_trial_list_(NULL) {
  }

  virtual ~VariationsUtilTest() {
    // Ensure that the maps are cleared between tests, since they are stored as
    // process singletons.
    testing::ClearAllVariationIDs();
  }

 private:
  base::FieldTrialList field_trial_list_;

  DISALLOW_COPY_AND_ASSIGN(VariationsUtilTest);
};

TEST_F(VariationsUtilTest, GetFieldTrialActiveGroups) {
  typedef std::set<ActiveGroupId, ActiveGroupIdCompare> ActiveGroupIdSet;
  std::string trial_one("trial one");
  std::string group_one("group one");
  std::string trial_two("trial two");
  std::string group_two("group two");

  base::FieldTrial::ActiveGroups active_groups;
  base::FieldTrial::ActiveGroup active_group;
  active_group.trial_name = trial_one;
  active_group.group_name = group_one;
  active_groups.push_back(active_group);

  active_group.trial_name = trial_two;
  active_group.group_name = group_two;
  active_groups.push_back(active_group);

  // Create our expected groups of IDs.
  ActiveGroupIdSet expected_groups;
  ActiveGroupId name_group_id;
  name_group_id.name = metrics::HashName(trial_one);
  name_group_id.group = metrics::HashName(group_one);
  expected_groups.insert(name_group_id);
  name_group_id.name = metrics::HashName(trial_two);
  name_group_id.group = metrics::HashName(group_two);
  expected_groups.insert(name_group_id);

  std::vector<ActiveGroupId> active_group_ids;
  testing::TestGetFieldTrialActiveGroupIds(active_groups, &active_group_ids);
  EXPECT_EQ(2U, active_group_ids.size());
  for (size_t i = 0; i < active_group_ids.size(); ++i) {
    ActiveGroupIdSet::iterator expected_group =
        expected_groups.find(active_group_ids[i]);
    EXPECT_FALSE(expected_group == expected_groups.end());
    expected_groups.erase(expected_group);
  }
  EXPECT_EQ(0U, expected_groups.size());
}

}  // namespace chrome_variations
