<?php
/**
 * @file
 * migrate_movabletype.features.user_role.inc
 */

/**
 * Implements hook_user_default_roles().
 */
function migrate_movabletype_user_default_roles() {
  $roles = array();

  // Exported role: admin.
  $roles['admin'] = array(
    'name' => 'admin',
    'weight' => '2',
  );

  // Exported role: blogger.
  $roles['blogger'] = array(
    'name' => 'blogger',
    'weight' => '3',
  );

  return $roles;
}
