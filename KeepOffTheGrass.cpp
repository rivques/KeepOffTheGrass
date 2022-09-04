#include "pch.h"
#include "KeepOffTheGrass.h"


BAKKESMOD_PLUGIN(KeepOffTheGrass, "don't touch the grass", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void KeepOffTheGrass::onLoad()
{
	_globalCvarManager = cvarManager;
	//cvarManager->log("Plugin loaded!");
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	gameWrapper->HookEventPost("Function TAGame.Car_TA.SetVehicleInput",
		[this](...) { onTick(); });
	gameWrapper->HookEventPost("Function TAGame.Team_TA.PostBeginPlay", [this](...) {onNewMap(); });

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	cvarManager->log("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	cvarManager->log("the cvar with name: " + cvarName + " changed");
	//	cvarManager->log("the new value is:" + newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&KeepOffTheGrass::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&KeepOffTheGrass::FUNCTION, this, _1, _2, _3));
	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {Render(canvas); });


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	cvarManager->log("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&KeepOffTheGrass::YourPluginMethod, this);
}

void KeepOffTheGrass::onTick() {
	if (!enabled) {
		oldEnabled = false;
		return;
	};
	ServerWrapper sw = getSW();

	if (sw.IsNull()) return;
	if (!oldEnabled) {
		oldEnabled = true;
		onNewMap();
	}
	HandleCarsOnGrass(sw);
}

ServerWrapper KeepOffTheGrass::getSW() {
	if (gameWrapper->IsInOnlineGame()) {
		cvarManager->log("null server");
		return NULL;
	}
	if (gameWrapper->IsInGame()) {
		auto server = gameWrapper->GetGameEventAsServer();

		if (server.IsNull()) {
			cvarManager->log("null server");
		}

		return server;
	}
	cvarManager->log("no server");
	return NULL;
}

void KeepOffTheGrass::onNewMap()
{
	ServerWrapper sw = getSW();

	if (sw.IsNull()) return;

	std::string mapName = gameWrapper->GetCurrentMap();
	if (mapName == "UtopiaStadium_Dusk_P" || mapName == "UtopiaStadium_P") {
		LOG("On utopia!");
		std::filesystem::path mapImagePath = gameWrapper->GetDataFolder() / "assets" / "KeepOffTheGrass" / "UtopiaStadium_P.bmp";
		if (!std::filesystem::exists(mapImagePath)) {
			LOG("No map image found!");
			mapImage = nullptr;
			return;
		}
		LOG("Attempting to load image at {}", mapImagePath.generic_string());
		Bitmap* origMapImage = Bitmap::FromFile(mapImagePath.c_str());
		if (!origMapImage) {
			LOG("Null origmapimage!");
			mapImage = nullptr;
			return;
		}
		float horizontalScalingFactor = 4096 / (float)origMapImage->GetWidth();
		float verticalScalingFactor = 5120 / (float)origMapImage->GetHeight();
		mapImage = new Bitmap(4096, 5120);
		Graphics g(mapImage);
		g.ScaleTransform(horizontalScalingFactor, verticalScalingFactor);
		g.DrawImage(origMapImage, 0, 0);

	}
	else {
		mapImage = nullptr;
	}
}

void KeepOffTheGrass::HandleCarsOnGrass(ServerWrapper sw)
{
	if (!mapImage) return;
	ArrayWrapper<CarWrapper> cars = sw.GetCars();
	if (cars.IsNull()) { return; }
	for (int i = 0; i < cars.Count(); i++) {
		CarWrapper car = cars.Get(i);
		if (!car) continue;
		if (!(car.GetTeamNum2() == 0 ? enabledBlue : enabledOrange)) continue;
		PriWrapper pri = car.GetPRI();
		if (!pri) continue;
		if (car.GetLocation().Z < 40) {
			Color pixelColor;
			mapImage->GetPixel(abs(car.GetLocation().X), abs(car.GetLocation().Y), &pixelColor);
			// full red: not allowed on offence
			// full blue: not allowed on defence
			// full purple/white: not allowed
			// black/anything else: allowed
			// on offence or defence?
			bool onOffence = car.GetTeamNum2() == 0 ? car.GetLocation().Y > 0 : car.GetLocation().Y < 0;
			bool isAllowed = (onOffence ? pixelColor.GetRed() : pixelColor.GetBlue()) != 255;
			if (!isAllowed) {
				LOG("{} is not allowed, DEMOING", pri.GetPlayerName().ToString());
				car.Demolish2(car);
			}
		}
	}
	
}

void KeepOffTheGrass::Render(CanvasWrapper canvas)
{
	if (!enabled) return;
	if (!drawDebug) return;
	auto camera = gameWrapper->GetCamera();
	if (camera.IsNull()) { LOG("Null camera"); return; }
	if (!mapImage) { LOG("Null mapImage(render)"); return; }
	LOG("Rendering!");
	RT::Frustum frust{ canvas, camera };
	for (int y = 0; y < 5120; y += 30) {
		for (int x = 0; x < 4096; x += 30) {
			Color pixelColor;
			mapImage->GetPixel(x, y, &pixelColor);
			LinearColor colors{(int)pixelColor.GetR(), (int)pixelColor.GetG(), (int)pixelColor.GetB(), 255};
			canvas.SetColor(colors);
			RT::Line(Vector{ (float)x, (float)y, 100 }, Vector{ (float)x + 30, (float)y + 30, 100 }, 3).DrawWithinFrustum(canvas, frust);
		}
	}
}

void KeepOffTheGrass::onUnload()
{
	delete mapImage;
	GdiplusShutdown(gdiplusToken);
}