// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "UITexts.h"

const char* en_lang[] = {
	"File",
	"Open",
	"Save",
	"Save as",
	"Choose File",
	"Import Model",
	"Export Model",
	"Exit",
	"Window",
	"Setting",
	"Language",
	"English",
	"Chinese",
	"Light Mass",
	"Style Editor",
	"Help",
	"About",
	"Error?",
	"Ok",
	"Cancel",
	"Don't ask me next time?"
};

const char* cn_lang[] = {
	"文件",
	"打开",
	"保存",
	"另存为",
	"选择文件",
	"导入模型",
	"导出模型",
	"退出",
	"窗体",
	"设置",
	"语言",
	"英文",
	"中文",
	"光照烘培",
	"风格",
	"帮助",
	"关于",
	"错误?",
	"确认",
	"取消",
	"下一次不再显示?"
};

std::vector<std::string> cn_lang_utf8;

void ConvertGBKToUtf8(const char* strGBK)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len];
	memset(wszUtf8, 0, len);
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);

	cn_lang_utf8.push_back(szUtf8);
	delete[] szUtf8;
	delete[] wszUtf8;
}

void GetLangPackage(UILang::Lang type, std::vector<const char*>& langs) noexcept
{
	langs.clear();

	if (type == UILang::Lang::English)
	{
		for (auto& it : en_lang)
			langs.push_back(it);
	}
	else
	{
		if (cn_lang_utf8.empty())
		{
			for (auto& it : cn_lang)
				ConvertGBKToUtf8(it);
		}

		for (auto& it : cn_lang_utf8)
		{
			langs.push_back(it.c_str());
		}
	}
}