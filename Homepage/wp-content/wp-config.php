<?php
/**
 * In dieser Datei werden die Grundeinstellungen für WordPress vorgenommen.
 *
 * Zu diesen Einstellungen gehören: MySQL-Zugangsdaten, Tabellenpräfix,
 * Secret-Keys, Sprache und ABSPATH. Mehr Informationen zur wp-config.php gibt es auf der {@link http://codex.wordpress.org/Editing_wp-config.php
 * wp-config.php editieren} Seite im Codex. Die Informationen für die MySQL-Datenbank bekommst du von deinem Webhoster.
 *
 * Diese Datei wird von der wp-config.php-Erzeugungsroutine verwendet. Sie wird ausgeführt, wenn noch keine wp-config.php (aber eine wp-config-sample.php) vorhanden ist,
 * und die Installationsroutine (/wp-admin/install.php) aufgerufen wird.
 * Man kann aber auch direkt in dieser Datei alle Eingaben vornehmen und sie von wp-config-sample.php in wp-config.php umbenennen und die Installation starten.
 *
 * @package WordPress
 */

/**  MySQL Einstellungen - diese Angaben bekommst du von deinem Webhoster. */
/**  Ersetze database_name_here mit dem Namen der Datenbank, die du verwenden möchtest. */
define('DB_NAME', 'db443270');

/** Ersetze username_here mit deinem MySQL-Datenbank-Benutzernamen */
define('DB_USER', 'db443270');

/** Ersetze password_here mit deinem MySQL-Passwort */
define('DB_PASSWORD', 'ur7Q4XAYPNFh');

/** Ersetze localhost mit der MySQL-Serveradresse */
define('DB_HOST', 'mysql5.anysense.de');

/** Der Datenbankzeichensatz der beim Erstellen der Datenbanktabellen verwendet werden soll */
define('DB_CHARSET', 'utf8');

/** Der collate type sollte nicht geändert werden */
define('DB_COLLATE', '');

/**#@+
 * Sicherheitsschlüssel
 *
 * Ändere jeden KEY in eine beliebige, möglichst einzigartige Phrase. 
 * Auf der Seite {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service} kannst du dir alle KEYS generieren lassen.
 * Bitte trage für jeden KEY eine eigene Phrase ein. Du kannst die Schlüssel jederzeit wieder ändern, alle angemeldeten Benutzer müssen sich danach erneut anmelden.
 *
 * @seit 2.6.0
 */
define('AUTH_KEY',         '<TILb9o5t>JoT(Fdnp|}?x1MQ{LI}78^xloAy@Q(?bQ4B)6C,43XnGB+J$hI&msC');
define('SECURE_AUTH_KEY',  'YyTL36b}A<U-+`[u-k55=+sa?IugO}Q`,.Htx+Rq#h2@ojU*r{RsqB~h+dAG`w(C');
define('LOGGED_IN_KEY',    'nB;.<-u_YFk`f3y`)&29GDv+|I?hno$=3 eAL5!<$i8=%_,.)i~2M~hJ(t+jso<Y');
define('NONCE_KEY',        '>tUKi^}:b}<O0cX;e7G<Z6-7%xp#QA?oU 8pxsUYfHBIUkd;zWpiMTj;>5j@l<sa');
define('AUTH_SALT',        '6. vjs_V(Ck{&BZ livZ7k+B<X44f[7N]T1D323+q],=~RU>uXHTlW%}Qa$Qd/&j');
define('SECURE_AUTH_SALT', '6|}+dz>]dsh9eDkv26} 98+`:hXl!Y~Lw|*&-dM`vB^yTh-N,10ebx+5qmT}=|t,');
define('LOGGED_IN_SALT',   '<|HIDH` q5EZ+RVd8fS*]a=|:cq _p{H^kH5=D3vBV85FEK`Jw<>NpJ3!mH3%o3x');
define('NONCE_SALT',       '!ZY_bKM-PfAiftH%sUT8b_;~4MJ0&WP ]}wIH-Q!;E|*JV(n@AOSx=&G7;8i{l.=');

/**#@-*/

/**
 * WordPress Datenbanktabellen-Präfix
 *
 *  Wenn du verschiedene Präfixe benutzt, kannst du innerhalb einer Datenbank
 *  verschiedene WordPress-Installationen betreiben. Nur Zahlen, Buchstaben und Unterstriche bitte!
 */
$table_prefix  = 'wp_';

/**
 * WordPress Sprachdatei
 *
 * Hier kannst du einstellen, welche Sprachdatei benutzt werden soll. Die entsprechende
 * Sprachdatei muss im Ordner wp-content/languages vorhanden sein, beispielsweise de_DE.mo
 * Wenn du nichts einträgst, wird Englisch genommen.
 */
define('WPLANG', 'de_DE');

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 */
define('WP_DEBUG', false);
define('WP_DEBUG_LOG', false);
define('WP_DEBUG_DISPLAY', false);

/* That's all, stop editing! Happy blogging. */

/** Absolute path to the WordPress directory. */
if ( !defined('ABSPATH') )
	define('ABSPATH', dirname(__FILE__) . '/');

/** Sets up WordPress vars and included files. */
require_once(ABSPATH . 'wp-settings.php');
