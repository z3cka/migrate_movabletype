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
 * Movable Type Blog to Drupal term migration
 */
class MovabletypeTermMigration extends MovabletypeMigration {
  public function __construct() {
    parent::__construct();
    // Human-friendly description of your migration process. Be as detailed as you
    // like.
    $this->description = t('Migrate MT Blogs as taxonomy terms');

    // Create a map object for tracking the relationships between source rows
    // and their resulting Drupal objects. Usually, you'll use the MigrateSQLMap
    // class, which uses database tables for tracking. Pass the machine name
    // (BeerTerm) of this migration to use in generating map and message tables.
    // And, pass schema definitions for the primary keys of the source and
    // destination - we need to be explicit for our source, but the destination
    // class knows its schema already.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'blog_name' => array('type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'Blog Name',
        )
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    // Note that by ordering on style_parent, we guarantee root terms are
    // migrated first, so the parent_name mapping below will find that the
    // parent exists.
    $query = db_select('mtdb'.'.mt_blog', 'mtbl')
      ->fields('mtbl', array(
//        'blog_id',
        'blog_description',
        'blog_name')
      );
    // This sort assures that parents are saved before children.
//      ->orderBy('style_parent', 'ASC');

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query);

    // Set up our destination - terms in the blog_name vocabulary
    $this->destination = new MigrateDestinationTerm('blog_name');

    // Assign mappings TO destination fields FROM source fields. To discover
    // the names used in these calls, use the drush commands
    // drush migrate-fields-destination BeerTerm
    // drush migrate-fields-source BeerTerm
    $this->addFieldMapping('description', 'blog_description');
    $this->addFieldMapping('name', 'blog_name');
//    $this->addFieldMapping('tid', 'blog_id');

    // Documenting your mappings makes it easier for the whole team to see
    // exactly what the status is when developing a migration process.
//    $this->addFieldMapping('parent_name', 'style_parent')
//      ->description(t('The incoming style_parent field is the name of the term parent'));

    // Mappings are assigned issue groups, by which they are grouped on the
    // migration info page when the migrate_ui module is enabled. The default
    // is 'Done', indicating active mappings which need no attention. A
    // suggested practice is to use groups of:
    // Do Not Migrate (or DNM) to indicate source fields which are not being used,
    //  or destination fields not to be populated by migration.
    // Client Issues to indicate input from the client is needed to determine
    //  how a given field is to be migrated.
    // Implementor Issues to indicate that the client has provided all the
    //  necessary information, and now the implementor needs to complete the work.
//    $this->addFieldMapping(NULL, 'hoppiness')
//      ->description(t('This info will not be maintained in Drupal'))
//      ->issueGroup(t('DNM'));

    // Open mapping issues can be assigned priorities (the default is
    // MigrateFieldMapping::ISSUE_PRIORITY_OK). If you're using an issue
    // tracking system, and have defined issuePattern (see BasicExampleMigration
    // above), you can specify a ticket/issue number in the system on the
    // mapping and migrate_ui will link directory to it.
//    $this->addFieldMapping(NULL, 'region')
//      ->description('Will a field be added to the vocabulary for this?')
//      ->issueGroup(t('Client Issues'))
//      ->issuePriority(MigrateFieldMapping::ISSUE_PRIORITY_MEDIUM)
//      ->issueNumber(770064);

    // It is good practice to account for all source and destination fields
    // explicitly - this makes sure that everyone understands exactly what is
    // being migrated and what is not. Also, migrate_ui highlights unmapped
    // fields, or mappings involving fields not in the source and destination,
    // so if (for example) a new field is added to the destination field it's
    // immediately visible, and you can find out if anything needs to be
    // migrated into it.
    $this->addFieldMapping('format')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('weight')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('parent')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('parent_name')
      ->issueGroup(t('DNM'));
  }
}

/**
 * Movable Type Tags to Drupal term migration
 */
class MovabletypeTermTagsMigration extends MovabletypeMigration {
  public function __construct() {
    parent::__construct();

    $this->description = t('Migrate MT Tags as taxonomy terms');

    // Create a map object for tracking the relationships between source rows
    // and their resulting Drupal objects.
    $this->map = new MigrateSQLMap($this->machineName,
      array(
        'tag_id' => array('type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
          'description' => 'Tags',
        )
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // We are getting data from tables in the Drupal default database - first,
    // set up a query for this data.
    // Note that by ordering on style_parent, we guarantee root terms are
    // migrated first, so the parent_name mapping below will find that the
    // parent exists.
    $query = db_select('mtdb'.'.mt_tag', 'mtta')
      ->fields('mtta', array(
        'tag_id',
        'tag_name')
    );

    // Create a MigrateSource object, which manages retrieving the input data.
    $this->source = new MigrateSourceSQL($query);

    // Set up our destination - terms in the tags vocabulary
    $this->destination = new MigrateDestinationTerm('tags');

    // Assign mappings TO destination fields FROM source fields.
    $this->addFieldMapping('field_mt_tag_id', 'tag_id');
    $this->addFieldMapping('name', 'tag_name');

    // Documenting your mappings makes it easier for the whole team to see
    // exactly what the status is when developing a migration process.
//    $this->addFieldMapping('parent_name', 'style_parent')
//      ->description(t('The incoming style_parent field is the name of the term parent'));

    // Mappings are assigned issue groups, by which they are grouped on the
    // migration info page when the migrate_ui module is enabled. The default
    // is 'Done', indicating active mappings which need no attention. A
    // suggested practice is to use groups of:
    // Do Not Migrate (or DNM) to indicate source fields which are not being used,
    //  or destination fields not to be populated by migration.
    // Client Issues to indicate input from the client is needed to determine
    //  how a given field is to be migrated.
    // Implementor Issues to indicate that the client has provided all the
    //  necessary information, and now the implementor needs to complete the work.
//    $this->addFieldMapping(NULL, 'hoppiness')
//      ->description(t('This info will not be maintained in Drupal'))
//      ->issueGroup(t('DNM'));

    // Open mapping issues can be assigned priorities (the default is
    // MigrateFieldMapping::ISSUE_PRIORITY_OK). If you're using an issue
    // tracking system, and have defined issuePattern (see BasicExampleMigration
    // above), you can specify a ticket/issue number in the system on the
    // mapping and migrate_ui will link directory to it.
//    $this->addFieldMapping(NULL, 'region')
//      ->description('Will a field be added to the vocabulary for this?')
//      ->issueGroup(t('Client Issues'))
//      ->issuePriority(MigrateFieldMapping::ISSUE_PRIORITY_MEDIUM)
//      ->issueNumber(770064);

    // It is good practice to account for all source and destination fields
    // explicitly - this makes sure that everyone understands exactly what is
    // being migrated and what is not. Also, migrate_ui highlights unmapped
    // fields, or mappings involving fields not in the source and destination,
    // so if (for example) a new field is added to the destination field it's
    // immediately visible, and you can find out if anything needs to be
    // migrated into it.
    $this->addFieldMapping('description')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('format')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('weight')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('parent')
      ->issueGroup(t('DNM'));
    $this->addFieldMapping('parent_name')
      ->issueGroup(t('DNM'));
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
    $this->addFieldMapping('is_new')
      ->defaultValue(TRUE);
    $this->addFieldMapping('uid', 'author_id');
    $this->addFieldMapping('mail', 'author_email');
    $this->addFieldMapping('name', 'author_name');
    $this->addFieldMapping('field_name_full', 'author_nickname');
    $this->addFieldMapping('pass', 'author_password');
    // The migrate module automatically converts date/time strings to UNIX timestamps.
    $this->addFieldMapping('created', 'author_created_on');

    //set some defaults
    $this->addFieldMapping('role_names')
      ->defaultValue(array('blogger'));
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
    $this->addUnmigratedDestinations(array(
      'field_name_full:format',
      'field_name_full:language',
      'signature_format',
      'roles',
      'timezone',
//      'path',
      'init',
//      'is_new',
      )
    );
  }
}

/**
 * Movable Type blog post to Drupal node migration.
 */
class MovabletypeNodeMigration extends MovabletypeMigration {
  public function __construct() {
    parent::__construct();
    $this->description = t('MovableType Blog Posts');

    // You may optionally declare dependencies for your migration - other migrations
    // which should run first. In this case, terms assigned to our nodes and
    // the authors of the nodes should be migrated before the nodes themselves.
    $this->dependencies = array('MovabletypeTerm', 'MovabletypeUser');

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
    $query->leftJoin('mtdb'.'.mt_blog', 'mtbl', 'mten.entry_blog_id = mtbl.blog_id');
    $query->addField('mtbl', 'blog_name');

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

    $source_fields = array(
//      'entry_id' => t('The entry id in the MT database'),
      'categories_list' => t('list of categories for each entry'),
      'tags_list' => t('list of tags for each entry'),
    );

//    $this->source = new MigrateSourceSQL($query);
    $this->source = new MigrateSourceSQL($query, $source_fields);

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
    $this->addFieldMapping('field_blog_name', 'blog_name');
    $this->addFieldMapping('field_categories', 'categories_list');
    $this->addFieldMapping('field_tags', 'tags_list');

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
    $this->addFieldMapping('body:format')
      ->defaultValue('html_full');
    $this->addFieldMapping('comment')
      ->defaultValue(2);
    $this->addFieldMapping('field_blog_name:create_term')
      ->defaultValue('true');
    $this->addFieldMapping('field_categories:create_term')
      ->defaultValue('true');
    $this->addFieldMapping('field_tags:create_term')
      ->defaultValue('true');

    // No unmapped source fields

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'body:language',
      'body:summary',
      'field_blog_name:source_type',
      'field_categories:source_type',
      'field_tags:source_type',
      'status',
      'promote',
      'revision',
      'revision_uid',
      'sticky',
      'log',
      'language',
      'tnid',
      )
    );
  }
  public function prepareRow($current_row) {
    $source_id = $current_row->entry_id;
    $result = db_select('mtdb'.'.mt_objecttag', 'mtbota')
      ->fields('mtbota', array('objecttag_object_id', 'objecttag_tag_id'))
      ->condition('objecttag_object_id', $source_id)
      ->execute();
    foreach ($result as $row) {
      // grab table that has the names of the tags
      $tag_names = db_select('mtdb'.'.mt_tag', 'mtta')
        ->fields('mtta', array('tag_id', 'tag_name'))
        ->condition('tag_id', $row->objecttag_tag_id)
        ->execute();
      foreach ($tag_names as $tag_name) {
        $current_row->tags_list[] = $tag_name->tag_name;
      }
    }
    $result = db_select('mtdb'.'.mt_placement', 'mtpl')
      ->fields('mtpl', array('placement_entry_id', 'placement_category_id'))
      ->condition('placement_entry_id', $source_id)
      ->execute();
    foreach ($result as $row) {
      $cat_names = db_select('mtdb'.'.mt_category', 'mtca')
        ->fields('mtca', array('category_id', 'category_label'))
        ->condition('category_id', $row->placement_category_id)
        ->execute();
      foreach ($cat_names as $cat_name) {
        $current_row->categories_list[] = $cat_name->category_label;
      }
    }

    // We could also have used this function to decide to skip a row, in cases
    // where that couldn't easily be done through the original query. Simply
    // return FALSE in such cases.
    return TRUE;
  }
}

/**
 * Import items from the migrate_example_beer_comment table and make them into
 * Drupal comment objects.
 */
class MovabletypeCommentMigration extends MovabletypeMigration {
  public function __construct() {
    parent::__construct();
    $this->description = 'Comments on blog posts';
    $this->dependencies = array('MovabletypeUser', 'MovabletypeNode');
    $this->map = new MigrateSQLMap($this->machineName,
      array('comment_id' => array(
        'type' => 'int',
        'not null' => TRUE,
      )
      ),
      MigrateDestinationComment::getKeySchema()
    );
    $query = db_select('mtdb'.'.mt_comment', 'mtco')
      ->fields('mtco', array('comment_id',
//      'comment_author',
      'comment_email', 'comment_entry_id',
      'comment_text',
      'comment_created_on', 'comment_modified_on'));
//      ->orderBy('cid_parent', 'ASC');
    // uid is not in the comment table ... let's grab it from the mt_author table
    // *** the following is making an assumption that all comments are from blog authors ***
    $query->leftJoin('mtdb'.'.mt_author', 'mtau', 'mtco.comment_email = mtau.author_email');
    $query->addField('mtau', 'author_id');
    $query->addField('mtau', 'author_name');

    $this->source = new MigrateSourceSQL($query);
    $this->destination = new MigrateDestinationComment('comment_node_blog');

    // Mapped fields
//    $this->addSimpleMappings(array('name', 'subject', 'mail'));
    $this->addFieldMapping('mail', 'comment_email');
    $this->addFieldMapping('name', 'author_name');
    $this->addFieldMapping('status')
      ->defaultValue(COMMENT_PUBLISHED);

    // We preserved bid => nid ids during BeerNode import so simple mapping suffices.
    $this->addFieldMapping('nid', 'comment_entry_id');

    $this->addFieldMapping('uid', 'author_id')
      ->sourceMigration('MovabletypeUser')
      ->defaultValue(0);

//    $this->addFieldMapping('pid', 'cid_parent')
//      ->sourceMigration('BeerComment')
//      ->description('Parent comment.');

    $this->addFieldMapping('comment_body', 'comment_text');
    $this->addFieldMapping('created', 'comment_created_on');
    $this->addFieldMapping('changed', 'comment_modified_on');

    // No unmapped source fields
//    $this->addUnmigratedSources(array(
//      'comment_author',
//      'comment_email',
//      )
//    );

    // Unmapped destination fields
    $this->addUnmigratedDestinations(array(
      'hostname',
//      'created',
//      'changed',
      'thread',
      'homepage',
      'language',
      'comment_body:format',
      'comment_body:language',
      'pid',
      'subject',
      )
    );

    if (module_exists('path')) {
      $this->addFieldMapping('path')
        ->issueGroup(t('DNM'));
    }
  }
}
