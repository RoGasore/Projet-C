//+------------------------------------------------------------------+
//|           EA Multi-Signaux Version 1.3 Final                    |
//+------------------------------------------------------------------+
#property strict

#include <Modules_1_2/ForexLife.mqh>
#include <Modules_1_2/Oscillator.mqh>
#include <Modules_1_2/Filtre TK Boss.mqh>
#include <Modules_1_2/Decision.mqh>
// #include <Modules_1_2/Telegram.mqh> // SUPPRIMÉ

// Paramètres globaux
input string TrendName = "ForexLife Trend Direction";
input string OscillatorName = "CASH OSCILLATOR";
input string ArrowName = "CASH BACK ARROW";
input bool enableDebugMode = true;
// input string TelegramToken = ""; // SUPPRIMÉ
// input string TelegramChatID = ""; // SUPPRIMÉ
input int BougiesMaxArrow = 100;

// === Variables globales pour les modules ===
int handleFL = INVALID_HANDLE; // Pour ForexLife
long ancienneBougieOsc = 0;    // Pour Oscillator
string etatForexLife = "NONE"; // Pour ForexLife
string etatOscillator = "NONE"; // Pour Oscillator
string etatFiltre = "NONE"; // Pour Filtre

// === Variables pour Arrow (gestion directe dans le main EA) ===
int handleArrow = INVALID_HANDLE;
double BufferSell[];
double BufferBuy[];
datetime Temps[];
datetime lastArrowBarTime = 0;
string lastArrowType = "NONE";

// Expose l'état Arrow pour Decision
string etatArrow = "NONE";

//+------------------------------------------------------------------+
//| Initialisation globale                                           |
//+------------------------------------------------------------------+
int OnInit()
{
    Print("🎬 Initialisation complète...");
    Init_ForexLife();
    Init_Oscillator();
    Init_Filtre();
    Init_Arrow();
    Print("✅ Initialisation terminée - Attente du premier signal...");
    return INIT_SUCCEEDED;
}

void OnDeinit(const int reason)
{
    Print("🔚 Déinitialisation de l'EA...");
    if(handleArrow != INVALID_HANDLE) IndicatorRelease(handleArrow);
}

//+------------------------------------------------------------------+
//| Initialisation Arrow                                             |
//+------------------------------------------------------------------+
void Init_Arrow()
{
    if(handleArrow != INVALID_HANDLE) IndicatorRelease(handleArrow);
    handleArrow = iCustom(_Symbol, _Period, ArrowName);
    if(handleArrow == INVALID_HANDLE)
        Print("❌ Arrow : erreur de chargement.");
    ArraySetAsSeries(BufferSell, true);
    ArraySetAsSeries(BufferBuy, true);
    ArraySetAsSeries(Temps, true);
    lastArrowBarTime = 0;
    lastArrowType = "NONE";
    etatArrow = "NONE";
}

//+------------------------------------------------------------------+
//| Tick principal                                                   |
//+------------------------------------------------------------------+
void OnTick()
{
    // Mise à jour des modules
    Check_ForexLife();
    Check_Oscillator();
    Check_TKboss();
    Check_Arrow();
    
    // Logs globaux uniquement à la clôture d'une bougie
    static datetime lastBarTime = 0;
    datetime currentBarTime = iTime(_Symbol, _Period, 0);
    if(currentBarTime != lastBarTime) {
        lastBarTime = currentBarTime;
        DebugIndicateurs();
        MAJ_RectanglesGestionRisque(); // Mise à jour des rectangles à chaque clôture de bougie
    }
    
    // Décision globale (module Decision)
    PrendreDecisionGlobale();
    VerifierPositionsFermees();
}

//+------------------------------------------------------------------+
//| Gestion robuste du signal Arrow (inspirée du prototype)          |
//+------------------------------------------------------------------+
void Check_Arrow()
{
    if(handleArrow == INVALID_HANDLE)
    {
        Print("⚠️ Handle Arrow invalide, tentative de réinitialisation...");
        Init_Arrow();
        if(handleArrow == INVALID_HANDLE)
        {
            Print("❌ Impossible de réinitialiser le handle Arrow");
            etatArrow = "NONE";
            return;
        }
    }
    if (CopyBuffer(handleArrow, 0, 0, BougiesMaxArrow, BufferSell) <= 0 ||
        CopyBuffer(handleArrow, 1, 0, BougiesMaxArrow, BufferBuy) <= 0 ||
        CopyTime(_Symbol, _Period, 0, BougiesMaxArrow, Temps) <= 0)
    {
        Print("❌ Erreur lors du chargement des données Arrow");
        etatArrow = "NONE";
        return;
    }
    string newArrowType = "NONE";
    datetime newArrowBarTime = 0;
    for(int i = 1; i < BougiesMaxArrow; i++) // on évite la bougie courante
    {
        if(BufferSell[i] != 0 && BufferBuy[i] == 0)
        {
            newArrowType = "SELL";
            newArrowBarTime = Temps[i];
            break;
        }
        else if(BufferBuy[i] != 0 && BufferSell[i] == 0)
        {
            newArrowType = "BUY";
            newArrowBarTime = Temps[i];
            break;
        }
    }
    // Mise à jour uniquement si changement
    if(newArrowType != lastArrowType || newArrowBarTime != lastArrowBarTime)
    {
        lastArrowType = newArrowType;
        lastArrowBarTime = newArrowBarTime;
        etatArrow = newArrowType;
        if(etatArrow == "BUY")
            Print("🟢 Arrow : BUY");
        else if(etatArrow == "SELL")
            Print("🔴 Arrow : SELL");
        else
            Print("⏸️ Arrow : NEUTRE");
    }
}

//+------------------------------------------------------------------+
//| Logs globaux                                                     |
//+------------------------------------------------------------------+
void DebugIndicateurs()
{
    if(!enableDebugMode) return;
    Print("🔍 === DEBUG INDICATEURS ===");
    Print("📊 ForexLife: ", etatForexLife);
    Print("📊 Oscillator: ", etatOscillator);
    Print("📊 Arrow: ", etatArrow);
    Print("📊 Filtre: ", etatFiltre);
}
//+------------------------------------------------------------------+ 