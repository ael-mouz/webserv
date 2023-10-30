/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MimeTypes.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yettabaa <yettabaa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/12 14:59:54 by ael-mouz          #+#    #+#             */
/*   Updated: 2023/10/30 16:23:18 by yettabaa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../Include/Config/MimeTypes.hpp"

MimeTypes::MimeTypes()
{
	mimeTypes_["html"] = "text/html";
	mimeTypes_["htm"] = "text/html";
	mimeTypes_["shtml"] = "text/html";
	mimeTypes_["css"] = "text/css";
	mimeTypes_["xml"] = "text/xml";
	mimeTypes_["gif"] = "image/gif";
	mimeTypes_["jpeg"] = "image/jpeg";
	mimeTypes_["jpg"] = "image/jpeg";
	mimeTypes_["js"] = "application/javascript";
	mimeTypes_["atom"] = "application/atom+xml";
	mimeTypes_["rss"] = "application/rss+xml";
	mimeTypes_["mml"] = "text/mathml";
	mimeTypes_["txt"] = "text/plain";
	mimeTypes_["jad"] = "text/vnd.sun.j2me.app-descriptor";
	mimeTypes_["wml"] = "text/vnd.wap.wml";
	mimeTypes_["htc"] = "text/x-component";
	mimeTypes_["avif"] = "image/avif";
	mimeTypes_["png"] = "image/png";
	mimeTypes_["svgz"] = "image/svg+xml";
	mimeTypes_["svg"] = "image/svg+xml";
	mimeTypes_["tif"] = "image/tiff";
	mimeTypes_["tiff"] = "image/tiff";
	mimeTypes_["wbmp"] = "image/vnd.wap.wbmp";
	mimeTypes_["webp"] = "image/webp";
	mimeTypes_["ico"] = "image/x-icon";
	mimeTypes_["jng"] = "image/x-jng";
	mimeTypes_["bmp"] = "image/x-ms-bmp";
	mimeTypes_["woff"] = "font/woff";
	mimeTypes_["woff2"] = "font/woff2";
	mimeTypes_["jar"] = "application/java-archive";
	mimeTypes_["war"] = "application/java-archive";
	mimeTypes_["ear"] = "application/java-archive";
	mimeTypes_["json"] = "application/json";
	mimeTypes_["hqx"] = "application/mac-binhex40";
	mimeTypes_["doc"] = "application/msword";
	mimeTypes_["pdf"] = "application/pdf";
	mimeTypes_["ps"] = "application/postscript";
	mimeTypes_["eps"] = "application/postscript";
	mimeTypes_["ai"] = "application/postscript";
	mimeTypes_["rtf"] = "application/rtf";
	mimeTypes_["m3u8"] = "application/vnd.apple.mpegurl";
	mimeTypes_["kml"] = "application/vnd.google-earth.kml+xml";
	mimeTypes_["kmz"] = "application/vnd.google-earth.kmz";
	mimeTypes_["xls"] = "application/vnd.ms-excel";
	mimeTypes_["eot"] = "application/vnd.ms-fontobject";
	mimeTypes_["ppt"] = "application/vnd.ms-powerpoint";
	mimeTypes_["odg"] = "application/vnd.oasis.opendocument.graphics";
	mimeTypes_["odp"] = "application/vnd.oasis.opendocument.presentation";
	mimeTypes_["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	mimeTypes_["odt"] = "application/vnd.oasis.opendocument.text";
	mimeTypes_["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mimeTypes_["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mimeTypes_["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mimeTypes_["wmlc"] = "application/vnd.wap.wmlc";
	mimeTypes_["wasm"] = "application/wasm";
	mimeTypes_["7z"] = "application/x-7z-compressed";
	mimeTypes_["cco"] = "application/x-cocoa";
	mimeTypes_["jardiff"] = "application/x-java-archive-diff";
	mimeTypes_["jnlp"] = "application/x-java-jnlp-file";
	mimeTypes_["run"] = "application/x-makeself";
	mimeTypes_["pl"] = "application/x-perl";
	mimeTypes_["pm"] = "application/x-perl";
	mimeTypes_["prc"] = "application/x-pilot";
	mimeTypes_["pdb"] = "application/x-pilot";
	mimeTypes_["rar"] = "application/x-rar-compressed";
	mimeTypes_["rpm"] = "application/x-redhat-package-manager";
	mimeTypes_["sea"] = "application/x-sea";
	mimeTypes_["swf"] = "application/x-shockwave-flash";
	mimeTypes_["sit"] = "application/x-stuffit";
	mimeTypes_["tcl"] = "application/x-tcl";
	mimeTypes_["tk"] = "application/x-tcl";
	mimeTypes_["der"] = "application/x-x509-ca-cert";
	mimeTypes_["pem"] = "application/x-x509-ca-cert";
	mimeTypes_["crt"] = "application/x-x509-ca-cert";
	mimeTypes_["xpi"] = "application/x-xpinstall";
	mimeTypes_["xhtml"] = "application/xhtml+xml";
	mimeTypes_["xspf"] = "application/xspf+xml";
	mimeTypes_["zip"] = "application/zip";
	mimeTypes_["bin"] = "application/octet-stream";
	mimeTypes_["exe"] = "application/octet-stream";
	mimeTypes_["dll"] = "application/octet-stream";
	mimeTypes_["deb"] = "application/octet-stream";
	mimeTypes_["dmg"] = "application/octet-stream";
	mimeTypes_["iso"] = "application/octet-stream";
	mimeTypes_["img"] = "application/octet-stream";
	mimeTypes_["msi"] = "application/octet-stream";
	mimeTypes_["msp"] = "application/octet-stream";
	mimeTypes_["msm"] = "application/octet-stream";
	mimeTypes_["mid"] = "audio/midi";
	mimeTypes_["midi"] = "audio/midi";
	mimeTypes_["kar"] = "audio/midi";
	mimeTypes_["mp3"] = "audio/mpeg";
	mimeTypes_["ogg"] = "audio/ogg";
	mimeTypes_["m4a"] = "audio/x-m4a";
	mimeTypes_["ra"] = "audio/x-realaudio";
	mimeTypes_["3gpp"] = "video/3gpp";
	mimeTypes_["3gp"] = "video/3gpp";
	mimeTypes_["ts"] = "video/mp2t";
	mimeTypes_["mp4"] = "video/mp4";
	mimeTypes_["mpeg"] = "video/mpeg";
	mimeTypes_["mpg"] = "video/mpeg";
	mimeTypes_["mov"] = "video/quicktime";
	mimeTypes_["webm"] = "video/webm";
	mimeTypes_["flv"] = "video/x-flv";
	mimeTypes_["m4v"] = "video/x-m4v";
	mimeTypes_["mng"] = "video/x-mng";
	mimeTypes_["asx"] = "video/x-ms-asf";
	mimeTypes_["asf"] = "video/x-ms-asf";
	mimeTypes_["wmv"] = "video/x-ms-wmv";
	mimeTypes_["avi"] = "video/x-msvideo";
};

std::string MimeTypes::getMimeType(const std::string &extension) const
{
	std::map<std::string, std::string>::const_iterator it = mimeTypes_.find(extension);
	if (it != mimeTypes_.end())
		return it->second;
	return "application/octet-stream";
}

MimeTypes::~MimeTypes() {}