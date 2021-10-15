#include "android/ContentUtils.h"
#include <cassert>
#include "Url.h"
#include "android/ContentResolver.h"
#include "android/android_net_Uri.h"

using namespace Framework;
using namespace Framework::Android;

bool CContentUtils::IsContentPath(const fs::path& filePath)
{
	return filePath.string().find("content:/") == 0;
}

std::string CContentUtils::BuildUriFromPath(const fs::path& filePath)
{
	assert(IsContentPath(filePath));
	auto uriPath = Framework::UrlEncode(filePath.string().substr(9));
	//Encode every slash after the last colon in the URI
	auto slashSearchPos = uriPath.rfind("%3A");
	assert(slashSearchPos != std::string::npos);
	while(1)
	{
		auto nextSlashPos = uriPath.find('/', slashSearchPos);
		if(nextSlashPos == std::string::npos) break;
		uriPath.replace(nextSlashPos, 1, "%2F");
		slashSearchPos += 3;
	}
	auto uri = "content://" + uriPath;
	return uri;
}

bool CContentUtils::DoesFileExist(const fs::path& filePath)
{
	assert(IsContentPath(filePath));
	auto env = CJavaVM::GetEnv();
	auto uri = BuildUriFromPath(filePath);
	auto& contentResolver = CContentResolver::GetInstance().GetContentResolver();
	auto pathUri = android::net::Uri::parse(env->NewStringUTF(uri.c_str()));
	try
	{
		auto cursor = contentResolver.query(pathUri, NULL, NULL, NULL, NULL, NULL);
		bool exists = cursor.getCount() > 0;
		cursor.close();
		return exists;
	}
	catch(const std::exception& e)
	{
		return false;
	}
}
