<?php

class XoopsUserMigration extends Migration {
  public function __construct() {
    parent::__construct();

    // $this->dependencies = array('ForumUser');

    // The example uses a SQL database as source.
    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_users', 'u')
            // ->condition('created_userid', 12341, '=')
            // ->condition('a.blog_posts_id', 2047, '=')
            // ->condition('pd.blog_parameter_key', 1, '=')
            // ->condition('pd.blog_parameter_sequence', 1, '=')
            ->fields('u', array('uid', 'loginname', 'email', 'pass'))
            ->fields('p', array('user_regdate', 'last_login', 'user_sig'));
    $query->leftJoin('xoops_user_profile', 'p', 'p.profileid = u.uid');

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationUser(array('md5_passwords' => TRUE));

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'uid' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 'u',
                         )
        ),
        MigrateDestinationUser::getKeySchema()
      );

    // Finally we add simple field mappings
    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('status')->defaultValue(TRUE);
    $this->addFieldMapping('name', 'loginname');
    $this->addFieldMapping('mail', 'email');
    $this->addFieldMapping('init', 'email');
    $this->addFieldMapping('pass', 'pass');
    $this->addFieldMapping('created', 'user_regdate');
    $this->addFieldMapping('login', 'last_login');
    $this->addFieldMapping('access', 'last_login');
    $this->addFieldMapping('timezone')->defaultValue("Asia/Taipei");
    $this->addFieldMapping('signature', 'user_sig');
    $this->addFieldMapping('signature_format')->defaultValue("bbcode");
    $this->addUnmigratedDestinations(array('roles', 'role_names', 'picture', 'language', 'theme', 'data', 'path'));
  }

  public function prepareRow($row)
  {
    $row->user_sig = mb_substr($row->user_sig, 0, 255);

    $result = db_query("SELECT uid, name, mail FROM {users} WHERE name = :name", array(':name' => $row->loginname));
    if ($record = $result->fetch()) {
      $uid = $record->uid;
      $name = $record->name;
      $mail = $record->mail;
      db_update('users')
      ->fields(array(
        'name' => $name.'_'.$uid,
        'mail' => $mail.'_'.$uid,
      ))
      ->condition('uid', $uid, '=')
      ->execute();
    }
  }

  public function prepare($user, $row)
  {

  }

}

/**
*
*/
class XoopsForumMigration extends Migration
{

  public function __construct()
  {
    parent::__construct();

    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_bb_forums', 'f')
            // ->condition('created_userid', 12341, '=')
            ->fields('f', array('forum_id', 'forum_name', 'forum_desc', 'parent_forum', 'forum_order'));
            // ->fields('p', array('user_regdate', 'last_login', 'user_sig'));

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationTerm('forums');

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'forum_id' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 'f',
                         )
        ),
        MigrateDestinationTerm::getKeySchema()
      );

    // Finally we add simple field mappings
    // $this->addFieldMapping('tid', 'forum_id');
    $this->addFieldMapping('name', 'forum_name');
    $this->addFieldMapping('description', 'forum_desc');
    $this->addFieldMapping('parent', 'parent_forum')->sourceMigration($this->machineName);;
    $this->addFieldMapping('weight', 'forum_order');

    //will be processed in prepareRow()
    // $this->addFieldMapping('parent_name', 'parent_forum');

    $this->addUnmigratedDestinations(array('format', 'path'));
  }

  public function prepareRow($row)
  {
    //  $query = Database::getConnection('default', 'for_migration')
    //         ->select('xoops_bb_forums', 'f')
    //         ->condition('forum_id', $row->parent_forum, '=')
    //         ->fields('f', array('forum_name'))->range(0, 1);
    // $result = $query->execute();
    // $row->parent_name = $result->fetchField();

    // if(empty($row->parent_name)) {
    //   $row->parent_name = '<root>';
    // }
  }
}

class XoopsNodeMigration extends Migration {
  public function __construct() {
    parent::__construct();

    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_bb_topics', 't')
            // ->condition('created_userid', 12341, '=')
            ->fields('t', array('topic_id', 'topic_title', 'topic_poster', 'topic_time', 'forum_id'));
            // ->fields('p', array('user_regdate', 'last_login', 'user_sig'));

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationNode('forum');

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'topic_id' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 't',
                         )
        ),
        MigrateDestinationNode::getKeySchema()
      );

    // Finally we add simple field mappings
    $this->addFieldMapping('nid', 'topic_id');
    $this->addFieldMapping('is_new')->defaultValue(TRUE);
    $this->addFieldMapping('title', 'topic_title');
    $this->addFieldMapping('uid', 'topic_poster');
    $this->addFieldMapping('revision_uid', 'topic_poster');
    $this->addFieldMapping('created', 'topic_time');
    $this->addFieldMapping('changed', 'topic_time');
    $this->addFieldMapping('revision')->defaultValue(0);
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('promote')->defaultValue(0);
    $this->addFieldMapping('sticky')->defaultValue(0);
    $this->addFieldMapping('taxonomy_forums', 'forum_id')->sourceMigration('XoopsForum');
    $this->addFieldMapping('taxonomy_forums:source_type')->defaultValue('tid');
    $this->addFieldMapping('taxonomy_forums:create_term')->defaultValue(FALSE);
    $this->addFieldMapping('taxonomy_forums:ignore_case')->defaultValue(TRUE);
    $this->addFieldMapping('comment')->defaultValue(TRUE);
    $this->addFieldMapping('log')->defaultValue('content from Xoops migration');

    $this->addFieldMapping('body', 'body');
    $this->addFieldMapping('body:summary', 'body');
    $this->addFieldMapping('body:format')->defaultValue('bbcode');

    $this->addUnmigratedDestinations(array('translate', 'path', 'tnid', 'language', 'body:language'));

  }

  public function prepareRow($row)
  {
    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_bb_posts', 'p')
            ->condition('topic_id', $row->topic_id, '=')
            ->fields('pt', array('post_text'))
            ->orderBy('pt.post_id', 'ASC')
            ->range(0, 1);

    $query->leftJoin('xoops_bb_posts_text', 'pt', 'pt.post_id = p.post_id');

    $result = $query->execute();
    $row->body = $result->fetchField();
  }
}

class XoopsCommentMigration extends Migration {
  public function __construct() {
    parent::__construct();

    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_bb_posts', 'p')
            ->condition('p.pid', 0, '!=')
            ->fields('p', array('topic_id', 'post_id', 'uid', 'subject', 'post_time'))
            ->fields('pt', array('post_text'));

    $query->leftJoin('xoops_bb_posts_text', 'pt', 'pt.post_id = p.post_id');

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationComment('comment_node_forum');

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'post_id' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 'p',
                         )
        ),
        MigrateDestinationComment::getKeySchema()
      );

    // Finally we add simple field mappings
    $this->addFieldMapping('nid', 'topic_id')->sourceMigration('XoopsNode');
    // $this->addFieldMapping('cid', 'post_id');
    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('subject', 'subject');
    $this->addFieldMapping('comment_body', 'post_text');
    $this->addFieldMapping('comment_body:format')->defaultValue('bbcode');
    $this->addFieldMapping('created', 'post_time');
    $this->addFieldMapping('changed', 'post_time');
    $this->addFieldMapping('status')->defaultValue(1);
    $this->addFieldMapping('pid')->defaultValue(0);

    $this->addUnmigratedDestinations(array('language', 'comment_body:language', 'thread', 'hostname', 'name', 'mail', 'homepage'));

  }

  public function prepareRow($row)
  {
    $row->subject = mb_substr($row->subject, 0, 64);
  }
}

class XoopsMessageMigration extends Migration {
  public function __construct() {
    parent::__construct();

    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_priv_msgs', 'm')
            ->condition('from_userid', 1, '!=')
            ->fields('m', array('msg_id', 'subject', 'from_userid', 'to_userid', 'msg_time', 'msg_text'));
            // ->fields('p', array('user_regdate', 'last_login', 'user_sig'));

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationTable('pm_message');

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'msg_id' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 'm',
                         )
        ),
        MigrateDestinationTable::getKeySchema('pm_message')
      );

    // Finally we add simple field mappings
    $this->addFieldMapping('author', 'from_userid');
    $this->addFieldMapping('subject', 'subject');
    $this->addFieldMapping('body', 'msg_text');
    $this->addFieldMapping('format')->defaultValue('filtered_html');
    $this->addFieldMapping('timestamp', 'msg_time');
    $this->addFieldMapping('has_tokens')->defaultValue(0);

    $this->addUnmigratedDestinations(array('mid'));

  }

  public function prepareRow($row)
  {
    if($row->from_userid == $row->to_userid) {
      return FALSE;
    }
  }

  public function complete($entity, $row)
  {
    db_insert("pm_index")->fields(array(
      'mid' => $entity->mid,
      'thread_id' => $entity->mid,
      'recipient' => $row->from_userid,
      'is_new' => 0,
      'deleted' => 0,
      'type' => 'user',
    ))->execute();
    db_insert("pm_index")->fields(array(
      'mid' => $entity->mid,
      'thread_id' => $entity->mid,
      'recipient' => $row->to_userid,
      'is_new' => 0,
      'deleted' => 0,
      'type' => 'user',
    ))->execute();
  }
}

class XoopsUserPictureMigration extends Migration
{
  public function __construct()
  {
    parent::__construct();

    $query = Database::getConnection('default', 'for_migration')
            ->select('xoops_users', 'u')
            // ->condition('uid', 83, '=')
            ->condition('u.user_avatar', 'blank.gif', '!=')
            ->fields('u', array('uid', 'user_avatar'));

    $this->source = new MigrateSourceSQL($query);
    // We migrate into "page" nodes.
    $this->destination = new MigrateDestinationFile();

    // We instantiate the MigrateMap
    $this->map = new MigrateSQLMap($this->machineName,
        array(
          'uid' => array('type' => 'int',
                          'unsigned' => TRUE,
                          'not null' => TRUE,
                          'alias' => 'u',
                         )
        ),
        MigrateDestinationFile::getKeySchema()
      );

    // Finally we add simple field mappings
    $this->addFieldMapping('uid', 'uid');
    $this->addFieldMapping('value', 'user_avatar');
    $this->addFieldMapping('destination_dir')->defaultValue('public://legacy');
    $this->addFieldMapping('source_dir')->defaultValue('public://legacy');
    $this->addFieldMapping('destination_file', 'user_avatar');
    $this->addFieldMapping('preserve_files')->defaultValue(TRUE);
    $this->addFieldMapping('file_replace')->defaultValue(MigrateFile::FILE_EXISTS_REUSE);

    $this->addUnmigratedDestinations(array('timestamp'));
  }

  public function complete($entity, $row)
  {
    db_query("UPDATE {users} SET picture = :fid WHERE uid = :uid", array(
      ':fid' => $entity->fid,
      ':uid' => $row->uid,
    ));
  }
}
