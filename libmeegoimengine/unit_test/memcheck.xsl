<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
	<xsl:apply-templates/>
</xsl:template>

<xsl:param name="global.target" select="'Unspecified'"/>

<xsl:template match="valgrindoutput">
	<html>
		<head>
			<title>Valgrind Memtest</title>
		</head>
		<body style="font-family: sans-serif;">
			<h1>Memtest for <xsl:value-of select="$global.target"/></h1>
			<xsl:apply-templates/>
		</body>
	</html>
</xsl:template>

<xsl:template match="error">
	<table border="0" cellspacing="0" cellpadding="0" style="margin-top: 2ex;
		margin-bottom: 2ex; width: 100%;">
		<tr>
			<td colspan="3" style="background-color: #dddddd;
				padding-top: 1ex; padding-bottom: 1ex; padding-left: 1ex;">
				<b>
				<xsl:apply-templates select="kind"/>
				<xsl:text>: </xsl:text>
				<xsl:apply-templates select="what"/>
				</b>
			</td>
		</tr>
		<xsl:for-each select="stack/frame">
			<tr style="background-color: #ffffff; height: 1px">
				<td colspan="3">
					<xsl:text disable-output-escaping="yes"></xsl:text>
				</td>
			</tr>
			<xsl:variable name="css-color">
				<xsl:choose>
					<xsl:when test="position() mod 2 = 0">
						<xsl:value-of select="'#eeeeee'"/>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="'#f4f4f4'"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:variable>
			<xsl:for-each select="child::*">
				<tr style="background-color: {$css-color}">
					<td style="padding-right: 0.5em; text-align: right;">
						<b><xsl:value-of select="name()"/></b><xsl:text>: </xsl:text>
					</td>
					<td style="width: 100%; padding-left: 0.5em; padding-right: 0.5em">
						<xsl:value-of select="."/>
					</td>
				</tr>
			</xsl:for-each>
		</xsl:for-each>
	</table>
</xsl:template>

<xsl:template match="unique|tid|kind|what">
	<xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()">
	<xsl:value-of select="."/>
</xsl:template>

<xsl:template match="*|comment()"/>

</xsl:stylesheet>
