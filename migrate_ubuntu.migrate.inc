<?php
function migrate_ubuntu_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'XoopsUser' => array('class_name' => 'XoopsUserMigration'),
      'XoopsUserPicture' => array('class_name' => 'XoopsUserPictureMigration'),
      'XoopsForum' => array('class_name' => 'XoopsForumMigration'),
      'XoopsNode' => array('class_name' => 'XoopsNodeMigration'),
      'XoopsComment' => array('class_name' => 'XoopsCommentMigration'),
      'XoopsMessage' => array('class_name' => 'XoopsMessageMigration'),
    ),
  );
  return $api;
}
