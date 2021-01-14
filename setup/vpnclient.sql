/*
 Navicat Premium Data Transfer

 Source Server         : vpnclient
 Source Server Type    : MySQL
 Source Server Version : 50610
 Source Host           : localhost:3306
 Source Schema         : vpnclient

 Target Server Type    : MySQL
 Target Server Version : 50610
 File Encoding         : 65001

 Date: 14/01/2021 19:26:07
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for appicon
-- ----------------------------
DROP TABLE IF EXISTS `appicon`;
CREATE TABLE `appicon`  (
  `appname` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `icon` blob NULL,
  `icon_press` blob NULL,
  `icon_hover` blob NULL,
  PRIMARY KEY (`appname`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8 COLLATE = utf8_general_ci ROW_FORMAT = Compact;

SET FOREIGN_KEY_CHECKS = 1;
