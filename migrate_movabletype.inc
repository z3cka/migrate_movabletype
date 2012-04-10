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
      ->fields('mtau', array('author_id', 'author_email', 'author_name', 'author_nickname', 'author_password', 'author_created_on'));
    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationUser();

    // One good way to organize your mappings is in three groups - mapped fields,
    // unmapped source fields, and unmapped destination fields

    // Mapped fields
//    $this->addFieldMapping('is_new')
//      ->defaultValue(TRUE);
    $this->addFieldMapping('uid', 'author_id');
    $this->addFieldMapping('mail', 'author_email');
    $this->addFieldMapping('name', 'author_name');
    $this->addFieldMapping('field_name_full', 'author_nickname');
    $this->addFieldMapping('pass', 'author_password');
    // The migrate module automatically converts date/time strings to UNIX timestamps.
    $this->addFieldMapping('created', 'author_created_on');

    //set some defaults
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
    $this->addUnmigratedDestinations(array('signature_format', 'timezone', 'path', 'init', 'is_new'));
  }
}

/**
 * Movable Type blog post to Drupal node migration.
 */
class MovabletypeNodeMigration extends MovabletypeMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('MoveableType Blog Posts');

    // You may optionally declare dependencies for your migration - other migrations
    // which should run first. In this case, terms assigned to our nodes and
    // the authors of the nodes should be migrated before the nodes themselves.
    $this->dependencies = array('MovabletypeUser');

    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'entry_id' => array(
          'type' => 'int',
          'not null' => TRUE,
          'description' => 'Blog Post ID.',
          'alias' => 'mten',
        )
      ),
      MigrateDestinationNode::getKeySchema()
    );

    // We have a more complicated query. The Migration class fundamentally
    // depends on taking a single source row and turning it into a single
    // Drupal object, so how do we deal with zero or more terms attached to
    // each node? One way (demonstrated for MySQL) is to pull them into a single
    // comma-separated list.
    $query = db_select('mtdb'.'.mt_entry', 'mten')
      ->fields('mten', array('entry_id', 'entry_author_id', 'entry_title', 'entry_text', 'entry_created_on', 'entry_modified_on'));
//    $query->leftJoin('migrate_example_beer_topic_node', 'tb', 'b.bid = tb.bid');
    // Gives a single comma-separated list of related terms
//    $query->groupBy('tb.bid');
//    $query->addExpression('GROUP_CONCAT(tb.style)', 'terms');

    // By default, MigrateSourceSQL derives a count query from the main query -
    // but we can override it if we know a simpler way
//    $count_query = db_select('migrate_example_beer_node', 'b');
//    $count_query->addExpression('COUNT(bid)', 'cnt');

    // Passing the cache_counts option means the source count (shown in
    // drush migrate-status) will be cached - this can be very handy when
    // dealing with a slow source database.
//    $this->source = new MigrateSourceSQL($query, array(), $count_query,
//      array('cache_counts' => TRUE));
    $this->source = new MigrateSourceSQL($query);

    // Set up our destination - nodes of type migrate_example_beer
    $this->destination = new MigrateDestinationNode('blog', array('text_format' => 'html_full'));

    // Mapped fields
    $this->addFieldMapping('title', 'entry_title')
      ->description(t('Blog post title in source to node title'));
//    $this->addFieldMapping('sticky')
//      ->description(t('Should we default this to 0 or 1?'))
//      ->issueGroup(t('Client questions'))
//      ->issueNumber(765736)
//      ->issuePriority(MigrateFieldMapping::ISSUE_PRIORITY_LOW);
    $this->addFieldMapping('body', 'entry_text');
    $this->addFieldMapping('created', 'entry_created_on');
    $this->addFieldMapping('changed', 'entry_modified_on');

    // To maintain node identities between the old and new systems (i.e., have
    // the same unique IDs), map the ID column from the old system to nid and
    // set is_new to TRUE.
    $this->addFieldMapping('nid', 'entry_id')
      ->description(t('Preserve old blog post ID as nid in Drupal'));
    $this->addFieldMapping('is_new')
      ->defaultValue(TRUE);

    // References to related objects (such as the author of the content) are
    // most likely going to be identifiers from the source data, not Drupal
    // identifiers (such as uids). You can use the mapping from the relevant
    // migration to translate from the old ID to the Drupal identifier.
    // Note that we also provide a default value of 1 - if the lookup fails to
    // find a corresponding uid for the aid, the owner will be the administrative
    // account.
    $this->addFieldMapping('uid', 'entry_author_id')
      ->sourceMigration('MovabletypeUser')
      ->defaultValue(1);

    // This is a multi-value text field
//    $this->addFieldMapping('field_migrate_example_country', 'countries')
//      ->separator('|');
    // These are related terms, which by default will be looked up by name
//    $this->addFieldMapping('migrate_example_beer_styles', 'terms')
//      ->separator(',');

    // Drupal 7 fields can have attributes such as text formats, or excerpts
    // (equivalent to teasers in previous Drupal versions).
    // The source_field argument specifies what field in the source query maps
    // to the corresponding attribute (excerpt in this case)
//    $arguments = MigrateTextFieldHandler::arguments(array('source_field' => 'excerpt'));
//    $this->addFieldMapping('body', 'body')
//      ->arguments($arguments);
    // Since the excerpt is mapped via an argument, add a null mapping so it's
    // not flagged as unmapped
//    $this->addFieldMapping(NULL, 'excerpt');

    // Copy an image file, write DB record to files table, and save in Field storage.
    // Note we specify the source query fields that will map to the file alt/title/description
    // values.
//    $arguments = MigrateFileFieldHandler::arguments(drupal_get_path('module', 'migrate_example'),
//      'file_copy', FILE_EXISTS_RENAME, NULL, array('source_field' => 'image_alt'),
//      array('source_field' => 'image_title'), array('source_field' => 'image_description'));
//    $this->addFieldMapping('field_migrate_example_image', 'image')
//      ->arguments($arguments);

    // set some defaults
    $this->addFieldMapping('comment')
      ->defaultValue(2);

    // No unmapped source fields

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array('status', 'promote', 'revision', 'revision_uid', 'sticky', 'log', 'language', 'tnid', 'path'));
  }
}
