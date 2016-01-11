<?php

/**
 * @file
 * Contains NexteuropaMigrateAbstractMigration class.
 */

use Drupal\integration_consumer\Migrate\MigrateItemJSON;
use Drupal\integration_consumer\Migrate\AbstractMigration;

/**
 * Class IntegrationTestAbstractMigration.
 */
abstract class NexteuropaMigrateAbstractMigration extends AbstractMigration {

  /**
   * Define source key, to be used in setMap().
   *
   * @return array
   *    Get default source key definition.
   */
  public function getSourceKey() {
    return array(
      '_id' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * Constructor.
   *
   * @param string $type
   *    Document type, used to identify the correct fixture directory.
   * @param mixed $arguments
   *    Default constructor arguments.
   */
  public function __construct($type, $arguments) {
    parent::__construct($arguments);

    $base_path = self::getFixturesPath() . '/' . $type;
    $this->setSource(new MigrateSourceList(
      new \MigrateListJSON($base_path . '/list.json'),
      new MigrateItemJSON($base_path . '/document-:id.json', array()),
      array()
    ));
  }

  /**
   * Return Consumer tests fixtures path.
   *
   * @return string
   *    Fixtures path.
   */
  public static function getFixturesPath() {
    return drupal_get_path('module', 'Nexteuropa_migrate') . '/fixtures';
  }

  /**
   * Implements Migration::complete() callback.
   *
   * @param object $entity
   *    Entity object.
   * @param \stdClass $source_row
   *    Source row, as expected by Migrate class.
   */
  public function complete($entity, \stdClass $source_row) {
    parent::complete($entity, $source_row);
    // Set the publication through workbench_moderation when enabled.
    if (isset($entity->workbench_moderation)) {
      workbench_moderation_moderate($entity, 'published');
    }
    foreach ($source_row->getAvailableLanguages() as $language) {
      if ($language != $source_row->getDefaultLanguage()) {
        $entity_type = $this->getDestination()->getEntityType();
        // Generate aliases for the translations.
        if (module_exists('pathauto') && is_callable('pathauto_' . $entity_type . '_update_alias')) {
          call_user_func('pathauto_' . $entity_type . '_update_alias', $entity, 'update', array('language' => $language));
        }
      }
    }
  }

  /**
   * Create Stub.
   */
  protected function createStub(Migration $migration, array $source_id) {
    $item = array(
      'link_path' => '<front>',
      'link_title' => t('Stub for @id', array('@id' => $source_id[0])),
      'menu_name' => 'main-menu',
      'weight' => 0,
      'expanded' => 0,
    );

    // On saving, the $item is passed by reference and hence
    // is given other properties, including the item ID.
    $item_id = menu_link_save($item);
    if ($item_id) {
      return array($item_id);
    }
    else {
      return FALSE;
    }
  }

}
