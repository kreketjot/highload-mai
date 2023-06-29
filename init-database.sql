-- Users --
CREATE TABLE IF NOT EXISTS `User` (
  `login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
  `title` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
  PRIMARY KEY (`login`), KEY `fn` (`first_name`), KEY `ln` (`last_name`)
);
DESCRIBE User;
INSERT INTO User (login, first_name, last_name, email, title)
  VALUES('AndreyVinokurov', 'Андрей', 'Винокуров', 'kreketjot@gmail.com', 'Mr.');
INSERT INTO User (login, first_name, last_name, email, title)
  VALUES('IvanovIvan', 'Иван', 'Иванов', 'ivanovivan@gmail.com', 'Mr.');
SELECT * FROM User;


-- UserAuth --
CREATE TABLE IF NOT EXISTS `UserAuth` (
  `login` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL UNIQUE,
  `password` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL
);
DESCRIBE UserAuth;
INSERT INTO UserAuth (login, password)
  VALUES('AndreyVinokurov', 'super_password');
INSERT INTO UserAuth (login, password)
  VALUES('IvanovIvan', 'asdfjkl;');
SELECT * FROM UserAuth;


-- BlogPost --
CREATE TABLE IF NOT EXISTS `BlogPost` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `author` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `title` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `content` VARCHAR(2048) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
  PRIMARY KEY (`id`)
);
DESCRIBE BlogPost;
INSERT INTO BlogPost (author, title, content)
  VALUES('AndreyVinokurov', 'Привет, мир!', 'Mой первый пост!');
SELECT * FROM BlogPost;


-- ChatMessage --
CREATE TABLE IF NOT EXISTS `ChatMessage` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `sender` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `recipient` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `content` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
);
DESCRIBE ChatMessage;
INSERT INTO ChatMessage (sender, recipient, content)
  VALUES('AndreyVinokurov', 'IvanovIvan', 'Ваня, привет');
INSERT INTO ChatMessage (sender, recipient, content)
  VALUES('AndreyVinokurov', 'IvanovIvan', 'Го кататься в четверг');
INSERT INTO ChatMessage (sender, recipient, content)
  VALUES('IvanovIvan', 'AndreyVinokurov', 'Привет, погнали');
INSERT INTO ChatMessage (sender, recipient, content)
  VALUES('AndreyVinokurov', 'IvanovIvan', 'Супер!');
SELECT * FROM ChatMessage;

