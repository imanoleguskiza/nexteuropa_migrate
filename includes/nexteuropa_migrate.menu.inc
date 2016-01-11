<?php

/**
 * @file
 * Contains NexteuropaMigrateAbstractMigration class.
 */

/**
 * Class NexteuropaMenuAbstractMigrations.
 */
abstract class NexteuropaMenuAbstractMigration extends NexteuropaAbstractMigration {

  /**
   * Constructor of the migrate class for Menus.
   *
   * @param string $migration_key
   *   Key identifying the folder containing the datas to migrate.
   * @param array $arguments
   *   Properties of the migration class.
   */
  public function __construct($migration_key, $arguments = array()) {
    parent::__construct($migration_key, $arguments);

    $this->addFieldMapping('link_path', 'link');
    $this->addFieldMapping('link_title', 'title');
    $this->addFieldMapping('weight', 'weight');
    $this->addFieldMapping('customized', 'customized')->defaultValue(1);
    $this->addFieldMapping('language', 'default_language')->defaultValue('und');
  }

  /**
   * Set Translation for menu item.
   */
  public function complete($item, \stdClass $source_row) {
    $this->menuTranslations($item, $source_row);

    // Update a link to be external.
    db_update('menu_links')
      ->fields(array(
        'external' => 1,
      ))
      ->condition('mlid', $item->mlid)
      ->execute();
  }

  /**
   * Save field translations for the specified menu item.
   *
   * @param \stdClass $item
   *   Item to translate.
   * @param \stdClass $source_row
   *   Source info for the translation.
   */
  public function menuTranslations(\stdClass $item, \stdClass $source_row) {
    foreach ($source_row->getAvailableLanguages() as $language) {
      if ($language != $source_row->getDefaultLanguage()) {
        $source_row->setSourceValues($language);
        $mlid = $item->mlid;
        $translation = $source_row->title;
        $name = 'menu:item:' . $mlid . ':title';
        i18n_string_translation_update($name, $translation, $language);
      }
    }
  }

}

