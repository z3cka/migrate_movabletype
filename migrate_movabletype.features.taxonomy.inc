<?php
/**
 * @file
 * migrate_movabletype.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function migrate_movabletype_taxonomy_default_vocabularies() {
  return array(
    'blog_name' => array(
      'name' => 'Blog Name',
      'machine_name' => 'blog_name',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'categories_blog' => array(
      'name' => 'Blog Categories',
      'machine_name' => 'categories_blog',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
    'tags' => array(
      'name' => 'Tags',
      'machine_name' => 'tags',
      'description' => '',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
    ),
  );
}
