<?php
/**
 * Created by JetBrains PhpStorm.
 * User: cgrzecka
 * Date: 4/5/12
 * Time: 10:42 AM
 * @file
 * Migration of a Moveable Type blog into drupal
 */

/**
 * Migration class to build from.
 */
abstract class MovabletypeMigration extends Migration {
  public function __construct() {
    // Always call the parent constructor first for basic setup
    parent::__construct();

    // With migrate_ui enabled, migration pages will indicate people involved in
    // the particular migration, with their role and contact info. We default the
    // list in the shared class; it can be overridden for specific migrations.
    $this->team = array(
      new MigrateTeamMember('Casey Grzecka', 'casey.grzecka@unlv.edu', t('Developer')),
    );

    // Below kept for reference
    // Individual mappings in a migration can be linked to a ticket or issue
    // in an external tracking system. Define the URL pattern here in the shared
    // class with ':id:' representing the position of the issue number, then add
    // ->issueNumber(1234) to a mapping.
//    $this->issuePattern = 'http://drupal.org/node/:id:';
  }
}

/**
 * Movable Type author to Drupal user migration.
 */
class MovabletypeUserMigration extends MovabletypeMigration {
  public function __construct() {
    // The basic setup is similar to BeerTermMigraiton
    parent::__construct();
    $this->description = t('Movable Type blog authors');
    $this->map = new MigrateSQLMap($this->machineName,
      array('author_id' => array(
        'type' => 'int',
        'not null' => TRUE,
        'description' => 'User ID.'
      )
      ),
      MigrateDestinationUser::getKeySchema()
    );
    $query = db_select('mtdb'.'.mt_author', 'mtau')
      ->fields('mtau', array('author_id', 'author_email', 'author_name', 'author_password', 'author_created_on'));
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationUser();

    // One good way to organize your mappings is in three groups - mapped fields,
    // unmapped source fields, and unmapped destination fields

    // Mapped fields
    $this->addFieldMapping('is_new')
      ->defaultValue(TRUE);
    $this->addFieldMapping('uid', 'author_id');
    $this->addFieldMapping('mail', 'author_email');
    $this->addFieldMapping('name', 'author_name');
    $this->addFieldMapping('pass', 'author_password');
    // The migrate module automatically converts date/time strings to UNIX timestamps.
    $this->addFieldMapping('created', 'author_created_on');

    //set som defaults
    $this->addFieldMapping('roles')
      ->defaultValue(4);
    $this->addFieldMapping('status')
      ->defaultValue(1);
    $this->addFieldMapping('theme')
      ->defaultValue('bartik');
    $this->addFieldMapping('signature')
      ->defaultValue('default');
    $this->addFieldMapping('access')
      ->defaultValue(0);
    $this->addFieldMapping('login')
      ->defaultValue(0);
    $this->addFieldMapping('language')
      ->defaultValue('en');
    $this->addFieldMapping('picture')
      ->defaultValue(0);

    // Unmapped source fields
    // ^^ none in this module, below is for reference only
//    $this->addFieldMapping(NULL, 'nickname')
//      ->issueGroup(t('DNM'));

    // Unmapped destination fields

    // This is a shortcut you can use to mark several destination fields as DNM
    // at once
    $this->addUnmigratedDestinations(array('signature_format', 'timezone', 'path', 'init'));
  }
}
