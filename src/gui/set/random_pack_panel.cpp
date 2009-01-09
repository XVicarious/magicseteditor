//+----------------------------------------------------------------------------+
//| Description:  Magic Set Editor - Program to make Magic (tm) cards          |
//| Copyright:    (C) 2001 - 2008 Twan van Laarhoven and "coppro"              |
//| License:      GNU General Public License 2 or later (see file COPYING)     |
//+----------------------------------------------------------------------------+

// ----------------------------------------------------------------------------- : Includes

#include <util/prec.hpp>
#include <gui/set/random_pack_panel.hpp>
#include <gui/set/window.hpp>
#include <gui/control/card_viewer.hpp>
#include <gui/control/filtered_card_list.hpp>
#include <data/game.hpp>
#include <data/pack.hpp>
#include <data/settings.hpp>
#include <util/window_id.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <wx/spinctrl.h>
#include <wx/dcbuffer.h>

DECLARE_TYPEOF_COLLECTION(PackTypeP);
DECLARE_TYPEOF_COLLECTION(PackItemP);
#if !USE_NEW_PACK_SYSTEM
	DECLARE_TYPEOF_COLLECTION(PackItemRefP);
#endif
DECLARE_TYPEOF_COLLECTION(CardP);
DECLARE_TYPEOF_COLLECTION(RandomPackPanel::PackItem_for_typeof);

// ----------------------------------------------------------------------------- : RandomCardList

/// A card list that contains the 
class RandomCardList : public CardListBase {
  public:
	RandomCardList(Window* parent, int id, long style = 0);
	
	/// Reset the list
	void reset();
  #if !USE_NEW_PACK_SYSTEM
	/// Add a pack of cards
	void add(PackItemCache& packs, boost::mt19937& gen, const PackType& pack);
  #endif
	
	using CardListBase::rebuild;
	
	const vector<CardP>* getCardsPtr() const { return &cards; }
	
  protected:
	virtual void getItems(vector<VoidP>& out) const;
	virtual void onChangeSet();
	
#if USE_NEW_PACK_SYSTEM
  public:
#else
  private:
#endif
	vector<CardP> cards;
};

RandomCardList::RandomCardList(Window* parent, int id, long style)
	: CardListBase(parent, id, style)
{}

void RandomCardList::reset() {
	cards.clear();
}

#if !USE_NEW_PACK_SYSTEM
	void RandomCardList::add(PackItemCache& packs, boost::mt19937& gen, const PackType& pack) {
		pack.generate(packs,gen,cards);
	}
#endif

void RandomCardList::onChangeSet() {
	reset();
	CardListBase::onChangeSet();
}

void RandomCardList::getItems(vector<VoidP>& out) const {
	out.reserve(cards.size());
	FOR_EACH_CONST(c, cards) {
		out.push_back(c);
	}
}


// ----------------------------------------------------------------------------- : PackTotalsPanel

class PackTotalsPanel : public wxPanel {
  public:
   #if USE_NEW_PACK_SYSTEM
	PackTotalsPanel(Window* parent, int id, PackGenerator& generator) : wxPanel(parent,id), generator(generator) {}
   #else
	PackTotalsPanel(Window* parent, int id) : wxPanel(parent,id) {}
   #endif
	void setGame(const GameP& game);
   #if !USE_NEW_PACK_SYSTEM
	void clear();
	void addPack(PackType& pack, int copies);
	void addItemRef(PackItemRef& item, int copies);
   #endif
  private:
	DECLARE_EVENT_TABLE();
	GameP game;
	void onPaint(wxPaintEvent&);
	void draw(DC& dc);
	void drawItem(DC& dc, int& y, const String& name, double value);
  #if USE_NEW_PACK_SYSTEM
	PackGenerator& generator;
  #else
	map<String,int> amounts;
  #endif
};

void PackTotalsPanel::onPaint(wxPaintEvent&) {
	wxBufferedPaintDC dc(this);
	draw(dc);
}
void PackTotalsPanel::draw(DC& dc) {
	// clear background
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.SetBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
	wxSize size = dc.GetSize();
	dc.DrawRectangle(0,0,size.x,size.y);
	// draw table
	dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dc.SetFont(*wxNORMAL_FONT);
	int y = 0;
	int total = 0;
  #if USE_NEW_PACK_SYSTEM
	FOR_EACH(pack, game->pack_types) {
		PackInstance& i = generator.get(pack);
		if (pack->summary && i.has_cards()) {
			drawItem(dc, y, tr(*game, pack->name, capitalize), i.get_card_copies());
			total += (int)i.get_card_copies();
		}
	}
  #else
	FOR_EACH(item, game->pack_items) {
		int value = amounts[item->name];
		drawItem(dc, y, tr(*game, item->name, capitalize), value);
		total += value;
	}
  #endif
	// draw total
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW));
	dc.DrawLine(0, y-3, size.x, y-3);
	dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT));
	dc.DrawLine(0, y-2, size.x, y-2);
	y += 7;
	drawItem(dc, y, _LABEL_("total cards"), total);
	
}
void PackTotalsPanel::drawItem(DC& dc, int& y,  const String& name, double value) {
	wxSize size = dc.GetSize();
	int w,h;
	String amount = String::Format(_("%.f"),value);
	dc.GetTextExtent(amount,&w,&h);
	dc.DrawText(name,   0,        y);
	dc.DrawText(amount, size.x-w, y);//align right
	y += h + 10;
}

void PackTotalsPanel::setGame(const GameP& game) {
	this->game = game;
  #if !USE_NEW_PACK_SYSTEM
	clear();
  #endif
}
#if !USE_NEW_PACK_SYSTEM
	void PackTotalsPanel::clear() {
		amounts.clear();
	}
	void PackTotalsPanel::addPack(PackType& pack, int copies) {
		FOR_EACH(item,pack.items) {
			addItemRef(*item, copies * item->amount);
		}
	}
	void PackTotalsPanel::addItemRef(PackItemRef& item, int copies) {
		amounts[item.name] += copies;
	}
#endif

BEGIN_EVENT_TABLE(PackTotalsPanel, wxPanel)
	EVT_PAINT(PackTotalsPanel::onPaint)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------- : RandomPackPanel

RandomPackPanel::RandomPackPanel(Window* parent, int id)
	: SetWindowPanel(parent, id)
{
	// delayed initialization by initControls()
}

void RandomPackPanel::initControls() {
	// init controls
	preview   = new CardViewer(this, wxID_ANY);
	card_list = new RandomCardList(this, wxID_ANY);
	generate_button = new wxButton(this, ID_GENERATE_PACK, _BUTTON_("generate pack"));
	seed_random = new wxRadioButton(this, ID_SEED_RANDOM, _BUTTON_("random seed"));
	seed_fixed  = new wxRadioButton(this, ID_SEED_FIXED,  _BUTTON_("fixed seed"));
	seed = new wxTextCtrl(this, wxID_ANY);
  #if USE_NEW_PACK_SYSTEM
	totals = new PackTotalsPanel(this, wxID_ANY, generator);
  #else
	totals = new PackTotalsPanel(this, wxID_ANY);
  #endif
	static_cast<SetWindow*>(GetParent())->setControlStatusText(seed_random, _HELP_("random seed"));
	static_cast<SetWindow*>(GetParent())->setControlStatusText(seed_fixed,  _HELP_("fixed seed"));
	static_cast<SetWindow*>(GetParent())->setControlStatusText(seed,        _HELP_("seed"));
	// init sizer
	wxSizer* s = new wxBoxSizer(wxHORIZONTAL);
		s->Add(preview, 0, wxRIGHT,  2);
		wxSizer* s2 = new wxBoxSizer(wxVERTICAL);
			wxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
				wxSizer* s4 = new wxStaticBoxSizer(wxHORIZONTAL, this, _LABEL_("pack selection"));
					packsSizer = new wxFlexGridSizer(0, 2, 4, 8);
					packsSizer->AddGrowableCol(0);
					//s4->AddSpacer(2);
					s4->Add(packsSizer, 1, wxEXPAND | wxALL & ~wxTOP, 4);
				s3->Add(s4, 1, wxEXPAND, 8);
				wxSizer* s5 = new wxStaticBoxSizer(wxHORIZONTAL, this, _LABEL_("pack totals"));
				s5->Add(totals, 1, wxEXPAND | wxALL, 4);
				s3->Add(s5, 1, wxEXPAND | wxLEFT, 8);
				wxSizer* s6 = new wxBoxSizer(wxVERTICAL);
					wxSizer* s7 = new wxStaticBoxSizer(wxVERTICAL, this, _LABEL_("seed"));
						s7->Add(seed_random, 0, wxALL, 4);
						s7->Add(seed_fixed,  0, wxALL, 4);
						wxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
							s8->Add(seed, 1, wxLEFT, 20);
						s7->Add(s8,          0, wxALL & ~wxTOP, 4);
					s6->Add(s7,       0, 0, 8);
					//s6->AddStretchSpacer();
					//s6->Add(generate_button, 0, wxTOP | wxALIGN_RIGHT, 8);
					s6->Add(generate_button, 1, wxTOP | wxEXPAND, 8);
				s3->Add(s6, 0, wxEXPAND | wxLEFT, 8);
			s2->Add(s3, 0, wxEXPAND | wxALL & ~wxTOP, 4);
			s2->Add(card_list, 1, wxEXPAND);
		s->Add(s2,      1, wxEXPAND, 8);
	s->SetSizeHints(this);
	SetSizer(s);
}

RandomPackPanel::~RandomPackPanel() {
	storeSettings();
}

void RandomPackPanel::onBeforeChangeSet() {
	if (set) {
		storeSettings();
	}
}
void RandomPackPanel::onChangeSet() {
	if (!isInitialized()) return;
	preview  ->setSet(set);
	card_list->setSet(set);
	totals   ->setGame(set->game);
	
	// remove old pack controls
	FOR_EACH(i, packs) {
		packsSizer->Detach(i.label);
		packsSizer->Detach(i.value);
		delete i.label;
		delete i.value;
	}
	packs.clear();
	
	// add pack controls
	FOR_EACH(pack, set->game->pack_types) {
	  #if NEW_PACK_SYSTEM
		if (pack->selectable) {
	  #endif
			PackItem i;
			i.pack  = pack;
			i.label = new wxStaticText(this, wxID_ANY, capitalize_sentence(pack->name));
			i.value = new wxSpinCtrl(this, ID_PACK_AMOUNT, _("0"), wxDefaultPosition, wxSize(50,-1));
			packsSizer->Add(i.label, 0, wxALIGN_CENTER_VERTICAL);
			packsSizer->Add(i.value, 0, wxEXPAND | wxALIGN_CENTER);
			packs.push_back(i);
	  #if NEW_PACK_SYSTEM
		}
	  #endif
	}
	
	Layout();
	
	// settings
	GameSettings& gs = settings.gameSettingsFor(*set->game);
	seed_random->SetValue(gs.pack_seed_random);
	seed_fixed ->SetValue(!gs.pack_seed_random);
	seed->Enable(!gs.pack_seed_random);
	setSeed(gs.pack_seed);
	FOR_EACH(i, packs) {
		i.value->SetValue(gs.pack_amounts[i.pack->name]);
	}
	
  #if USE_NEW_PACK_SYSTEM
	generator.reset(set,0);
  #endif
	updateTotals();
}

void RandomPackPanel::storeSettings() {
	if (!isInitialized()) return;
	GameSettings& gs = settings.gameSettingsFor(*set->game);
	gs.pack_seed_random = seed_random->GetValue();
	FOR_EACH(i, packs) {
		gs.pack_amounts[i.pack->name] = i.value->GetValue();
	}
}

// ----------------------------------------------------------------------------- : UI

void RandomPackPanel::initUI(wxToolBar* tb, wxMenuBar* mb) {
	// Init controls?
	if (!isInitialized()) {
		wxBusyCursor busy;
		initControls();
		onChangeSet();
	}
}

void RandomPackPanel::destroyUI(wxToolBar* tb, wxMenuBar* mb) {}

void RandomPackPanel::onUpdateUI(wxUpdateUIEvent& ev) {}

void RandomPackPanel::onCommand(int id) {
	if (!isInitialized()) return;
	switch (id) {
		case ID_PACK_AMOUNT: {
			updateTotals();
			break;
		}
		case ID_GENERATE_PACK: {
			generate();
			break;
		}
		case ID_SEED_RANDOM: case ID_SEED_FIXED: {
			seed->Enable(seed_fixed->GetValue());
			break;
		}
	}
}

// ----------------------------------------------------------------------------- : Generating

void RandomPackPanel::updateTotals() {
  #if !USE_NEW_PACK_SYSTEM
	totals->clear();
  #endif
	int total_packs = 0;
	FOR_EACH(i,packs) {
		int copies = i.value->GetValue();
		total_packs += copies;
	  #if USE_NEW_PACK_SYSTEM
		generator.get(i.pack).request_copy(copies);
	  #else
		totals->addPack(*i.pack, copies);
	  #endif
	}
  #if USE_NEW_PACK_SYSTEM
	generator.update_card_counts();
  #endif
	// update UI
	totals->Refresh(false);
	generate_button->Enable(total_packs > 0);
}

int RandomPackPanel::getSeed() {
	// determine seed value
	int seed = 0;
	if (seed_random->GetValue()) {
		// use the C rand() function to get a seed
		seed = rand() % 1000 * 1000
		     + clock() % 1000;
	} else {
		// convert *any* string to a number
		String s = this->seed->GetValue();
		FOR_EACH_CONST(c,s) {
			seed *= 10;
			seed += abs(c - '0') + 123456789*(abs(c - '0')/10);
		}
	}
	setSeed(seed);
	return seed;
}
void RandomPackPanel::setSeed(int seed) {
	seed %= 1000000;
	this->seed->SetValue(wxString::Format(_("%06d"),seed));
	GameSettings& gs = settings.gameSettingsFor(*set->game);
	gs.pack_seed = seed;
}

void RandomPackPanel::generate() {
  #if USE_NEW_PACK_SYSTEM
	generator.reset(set,getSeed());
  #else
	boost::mt19937 gen((unsigned)getSeed());
	PackItemCache pack_cache(*set);
  #endif
	// add packs to card list
	card_list->reset();
	FOR_EACH(item,packs) {
		int copies = item.value->GetValue();
		for (int i = 0 ; i < copies ; ++i) {
		  #if USE_NEW_PACK_SYSTEM
			generator.get(item.pack).request_copy();
			generator.generate(card_list->cards);
		  #else
			card_list->add(pack_cache, gen, *item.pack);
		  #endif
		}
	}
	card_list->rebuild();
	card_list->selectFirst();
}

// ----------------------------------------------------------------------------- : Selection

CardP RandomPackPanel::selectedCard() const {
	if (!isInitialized()) return CardP();
	return card_list->getCard();
}

void RandomPackPanel::selectCard(const CardP& card) {
	// Don't change the card based on other panels
	//preview->setCard(card);
}
void RandomPackPanel::onCardSelect(CardSelectEvent& ev) {
	preview->setCard(ev.getCard());
	ev.Skip(); // but do change other panels' selection
}

void RandomPackPanel::selectionChoices(ExportCardSelectionChoices& out) {
	if (!isInitialized()) return;
	out.push_back(new_intrusive2<ExportCardSelectionChoice>(
			_BUTTON_("export generated packs"),
			card_list->getCardsPtr()
		));
}


BEGIN_EVENT_TABLE(RandomPackPanel, wxPanel)
	EVT_CARD_SELECT(wxID_ANY, RandomPackPanel::onCardSelect)
END_EVENT_TABLE  ()


// ----------------------------------------------------------------------------- : Clipboard

bool RandomPackPanel::canCopy()  const { return isInitialized() && card_list->canCopy();  }
void RandomPackPanel::doCopy()         {        isInitialized() && card_list->doCopy();   }
