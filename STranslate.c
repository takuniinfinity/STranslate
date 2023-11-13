/*
 * STranslate (C)2023 Takuni Infinity
 * https://github.com/takuniinfinity/
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/cJSON.h"
#include <iup.h>
#include <curl/curl.h>

#define TITLE "STranslate"

struct {
	char id[4];
	char* name;
} country[] = {
	{"af", "Afrikaans"},
	{"am", "Amharic"},
	{"ar", "Arabic"},
	{"arn", "Mapudungun"},
	{"as", "Assamese"},
	{"az", "Azerbaijani"},
	{"ba", "Bashkir"},
	{"be", "Belarusian"},
	{"bg", "Bulgarian"},
	{"bn", "Bengali"},
	{"bo", "Tibetan"},
	{"br", "Breton"},
	{"bs", "Bosnian"},
	{"ca", "Catalan"},
	{"co", "Corsican"},
	{"cs", "Czech"},
	{"cy", "Welsh"},
	{"da", "Danish"},
	{"de", "German"},
	{"dsb", "Lower Sorbian"},
	{"dv", "Divehi"},
	{"el", "Greek"},
	{"en", "English"},
	{"es", "Spanish"},
	{"et", "Estonian"},
	{"eu", "Basque"},
	{"fa", "Persian"},
	{"fi", "Finnish"},
	{"fil", "Filipino"},
	{"fo", "Faroese"},
	{"fr", "French"},
	{"fy", "Frisian"},
	{"ga", "Irish"},
	{"gd", "Scottish Gaelic"},
	{"gl", "Galician"},
	{"gsw", "Swiss German"},
	{"gu", "Gujarati"},
	{"ha", "Hausa"},
	{"he", "Hebrew"},
	{"hi", "Hindi"},
	{"hr", "Croatian"},
	{"hsb", "Upper Sorbian"},
	{"hu", "Hungarian"},
	{"hy", "Armenian"},
	{"id", "Indonesia"},
	{"ig", "Igbo"},
	{"ii", "Yi"},
	{"is", "Icelandic"},
	{"it", "Italiano"},
	{"iu", "Inuktitut"},
	{"ja", "Japanese"},
	{"ka", "Georgian"},
	{"kk", "kazakh"},
	{"kl", "Greenlandic"},
	{"km", "Khamer"},
	{"kn", "Kannada"},
	{"ko", "Korean"},
	{"kok", "Konkani"},
	{"ckb", "Kurdi"},
	{"ky", "Kyrgyz"},
	{"lb", "Luxembourgish"},
	{"lo", "Lao"},
	{"lt", "Lithuania"},
	{"lv", "Latvian"},
	{"mi", "Maori"},
	{"mk", "Macedonian"},
	{"ml", "Malayalam"},
	{"mn", "Mongolian"},
	{"moh", "Mohawk"},
	{"mr", "Marathi"},
	{"ms", "Malay"},
	{"mt", "Maltese"},
	{"my", "Burmese"},
	{"nb", "Norwegian (Bokmal)"},
	{"ne", "Nepali"},
	{"nl", "Dutch"},
	{"nn", "Norwegian (Nynorsk)"},
	{"no", "Norwegian"},
	{"st", "Sesotho"},
	{"oc", "Occitan"},
	{"or", "Odia"},
	{"pa", "Punjabi"},
	{"pl", "Polish"},
	{"prs", "Dari"},
	{"ps", "Pashto"},
	{"pt", "Portugues"},
	{"quc", "K'iche'"},
	{"qu", "Quechua"},
	{"rm", "Romansh"},
	{"ru", "Russian"},
	{"rw", "Kinyarwanda"},
	{"sa", "Sanskrit"},
	{"sah", "Yakut"},
	{"se", "Sami (Northern)"},
	{"si", "Sinhala"},
	{"sk", "Slovak"},
	{"sl", "Slovenia"},
	{"sma", "Sami (Southern)"},
	{"smj", "Sami (Lule)"},
	{"smn", "Sami (Inari)"},
	{"sms", "Sami (Skolt)"},
	{"sq", "Albanian"},
	{"sr", "Serbian"},
	{"sv", "Swedish"},
	{"sw", "Kiswahili"},
	{"syc", "Syriac"},
	{"ta", "Tamil"},
	{"te", "Telugu"},
	{"tg", "Tajik"},
	{"th", "Thai"},
	{"tk", "Turkmen"},
	{"tn", "Tswana"},
	{"tr", "Turkish"},
	{"tt", "Tatar"},
	{"tzm", "Tamazight"},
	{"ug", "Uyghur"},
	{"uk", "Ukrainian"},
	{"ur", "Urdu"},
	{"uz", "Uzbek"},
	{"vi", "Vietnamese"},
	{"wo", "Wolof"},
	{"xh", "Xhosa"},
	{"yo", "Yoruba"},
	{"zh", "Chinese"},
	{"zu", "Zulu"},
};
const int country_count = 124;

#define url "https://api.mymemory.translated.net/get?q="
#define langpair "&langpair="
#define help "STranslate [from|to] [text] <- cli mode\nexample : STranslate id|en pesan\nSTranslate gui <- gui mode"

CURL* curl;
CURLcode res;
char* buffer;

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
	char** buffer = (char**)userdata;
	
	size_t total_size = size * nmemb;
	
	*buffer = realloc(*buffer, total_size + 1);
	memcpy(*buffer, ptr, total_size);
	(*buffer)[total_size] = '\0';
	
	return total_size;
}

char* translate(char* fto, char* from, char* to, char* text) {
	unsigned int langs;
	if (fto == NULL) {
		langs = strlen(from) + strlen(to) + 2;
	} else { langs = strlen(fto); }
	char lang[langs];
	if (fto == NULL) {
		strcpy(lang, from);
		strcat(lang, "|");
		strcat(lang, to);
	} else { strcpy(lang, fto); }
	
	char* encoded_text = curl_easy_escape(curl, text, 0);
	char full[strlen(url) + strlen(encoded_text) + strlen(langpair) + strlen(lang)];
	
	strcpy(full, url);
	strcat(full, encoded_text);
	strcat(full, langpair);
	strcat(full, lang);
	
	curl_easy_setopt(curl, CURLOPT_URL, full);
	res = curl_easy_perform(curl);
	//fprintf(stderr, "%s\n", curl_easy_strerror(res));
	if (res == CURLE_OK) {
		cJSON* json = cJSON_Parse(buffer);
		cJSON* responseData = cJSON_GetObjectItem(json, "responseData");
		cJSON* translatedText = cJSON_GetObjectItem(responseData, "translatedText");
		return cJSON_GetStringValue(translatedText);
	}
	return (char*)curl_easy_strerror(res);
}

void gui(int* argc, char** args[]) {
	Ihandle *dlg, *main_box, *appname, *link;
	Ihandle *textbox_from, * textbox_to;
	Ihandle *option_from, *option_to, *btn_translate;
	
	IupOpen(argc, args);
	
	appname = IupLabel("Simple Translate");
	IupSetAttribute(appname, "FONT", "Times Bold, 25");
	
	link = IupLink("https://takuniinfinity.github.io", "Takuni Infinity");
	
	textbox_from = IupMultiLine(NULL);
	IupSetAttribute(textbox_from, "WORDWRAP", "YES");
	IupSetAttribute(textbox_from, "EXPAND", "YES");

	textbox_to = IupMultiLine(NULL);
	IupSetAttribute(textbox_to, "WORDWRAP", "YES");
	IupSetAttribute(textbox_to, "EXPAND", "YES");
	IupSetAttribute(textbox_to, "READONLY", "YES");
	
	option_from = IupList(NULL);
	option_to = IupList(NULL);
	IupSetAttribute(option_from, "DROPDOWN", "YES");
	IupSetAttribute(option_to, "DROPDOWN", "YES");
	for (int i=0; i<country_count; i++) {
		char num[4];
		sprintf(num, "%i", i+1);
		IupSetStrAttribute(option_from, num, country[i].name);
		IupSetStrAttribute(option_to, num, country[i].name);
	}
	
	int btn_click(Ihandle* o) {
		int from = atoi(IupGetAttribute(option_from, "VALUE"));
		int to = atoi(IupGetAttribute(option_to, "VALUE"));
		if (!from || !to) {
			IupMessage("Error", "Please choose language from and to");
			return IUP_DEFAULT;
		}
		IupSetAttribute(textbox_to, "VALUE", 
			translate(
				NULL, country[from-1].id, country[to-1].id,
				IupGetAttribute(textbox_from, "VALUE")
			)
		);
		return IUP_DEFAULT;
	}
	btn_translate = IupButton("TRANSLATE", NULL);
	IupSetCallback(btn_translate, "ACTION", (Icallback)btn_click);
	
	main_box = IupVbox(
		IupHbox(IupFill(), appname, IupFill(), NULL),
		IupHbox(IupFill(), link, IupFill(), NULL),
		IupHbox(textbox_from, textbox_to, NULL),
		IupHbox(option_from, IupFill(), btn_translate, IupFill(), option_to, NULL), NULL
	);
	dlg = IupDialog(main_box);
	IupSetAttribute(dlg, "TITLE", TITLE);
	IupSetAttribute(dlg, "MARGIN", "10x10");
	IupSetAttribute(dlg, "SIZE", "QUARTERxQUARTER");
	IupShowXY(dlg, IUP_CENTER, IUP_CENTER);
	IupMainLoop();
	IupClose();
}

int main(int argc, char* args[]) {
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	#ifdef _WIN32
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	#endif
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
	
	if (argc == 2 && strcmp(args[1], "gui")==0) { // GUI
		gui(&argc, &args);
	} else if (argc == 3) { // CLI
		fprintf(stderr, "%s\n", translate(args[1], NULL, NULL, args[2]));
	} else { // INVALID
		fprintf(stderr, "%s\n", help);
	}
	
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if (buffer != NULL) { free(buffer); }
	return 0;
}
