// Mainframe macro generated from application: /disks/ekplx31/scratch/martschei/root_v18/root/bin/root.exe
// By ROOT version 5.18/00 on 2008-07-08 15:10:08

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGuiBldHintsButton
#include "TGuiBldHintsButton.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#ifndef ROOT_TFile
#include "TFile.h"
#endif
#ifndef ROOT_TH1F
#include "TH1F.h"
#endif

#include "Riostream.h"


#include <vector>

vector <TGCheckButton *> fTextButtons;
vector <TH1F*> Histograms;

void Plot(TCanvas* canvas) {
	canvas->Clear();
	int drawn=0;
	for(uint iButton = 0; iButton<fTextButtons.size(); iButton++) {
		//cout << " Button " << (fTextButtons.at(iButton))->GetString() << " is " << (fTextButtons.at(iButton))->IsDown() << endl;
		if((fTextButtons.at(iButton))->IsDown()){
			if(drawn){
				Histograms[iButton]->SetFillStyle( 3554 );
                		Histograms[iButton]->SetFillColor( 1+drawn );
				Histograms[iButton]->Draw("same");
			}
			else {
				Histograms[iButton]->SetFillStyle( 1001 );
				Histograms[iButton]->SetFillColor( 38 );
				Histograms[iButton]->Draw();
			}
			drawn++;
		}

	}

	//cout << "-----------" << endl;	
	canvas->Update();
}

void showHistos()
{
	TFile* HistoSourceFile = new TFile("evaluationsPlots.root", "READ");
	//TFile* HistoSourceFile = new TFile("teacherHistos.root", "READ");

	//HistoSourceFile->Print();
	//HistoSourceFile->GetListOfKeys()->Print();
	TList* KeyList = HistoSourceFile->GetListOfKeys();
	int nHistos = HistoSourceFile->GetListOfKeys()->GetEntries();

	//vector <TH1F*> Histograms;
	for (int ientry = 0; ientry < nHistos; ientry++) {
		TString histname(KeyList->At(ientry)->GetName());
		cout << " getting " << histname << endl;
		TH1F* histo = (TH1F*) HistoSourceFile->Get( histname );
		int maxVal = (int)histo->GetBinContent(histo->GetMaximumBin());
		cout << "maxBin has " <<  maxVal << endl;
		if(Histograms.size() > 1) {
			cout << "there are already two or more elements" << endl;
			for (vector<TH1F*>::iterator ihist=Histograms.begin()+1; ihist!=Histograms.end(); ihist++) {
				int firstval = (int)(*(ihist-1))->GetBinContent((*(ihist-1))->GetMaximumBin());
				int secondval = (int)(*ihist)->GetBinContent((*ihist)->GetMaximumBin());
				cout << firstval << " - " << secondval << " : "  << maxVal << endl << flush; 
				if( maxVal>firstval ) {
					Histograms.insert(Histograms.begin(),histo);
					cout << "inserting at first place" << endl; break;
				}
				else if(maxVal<=firstval && maxVal>secondval){ 
					Histograms.insert(ihist,histo);
					cout << "inserting between " << firstval << " and " << secondval  << endl;
					break;
				}
				
				else if (ihist==Histograms.end()-1) {
					Histograms.push_back(histo);
					cout << histname << " added to the end " << endl;
					break;
				}
			}
		}
		else if(Histograms.size() > 0) {
			if(maxVal>Histograms.front()->GetBinContent((Histograms.front())->GetMaximumBin()))  {
				Histograms.insert(Histograms.begin(),histo); cout << histname << " inserting before existing" << endl;
			}
			else {
				Histograms.push_back(histo);
				cout << histname << " added to the end " << endl;
			}

		}
			
		else {
			Histograms.push_back(histo);
			cout << histname << " added as first element " << endl;
		}
		//cout << histname << " added to vector " << 
		cout << endl << "----------------" << endl;	
	}

   // main frame
   TGMainFrame *fMainFrame = new TGMainFrame(0,10,10,kMainFrame | kVerticalFrame);
   //fMainFrame->SetLayoutBroken(kTRUE);

   // horizontal frame
   TGHorizontalFrame *fHorizontalFrame1 = new TGHorizontalFrame(fMainFrame,800,600,kHorizontalFrame);
   //fHorizontalFrame1->SetLayoutBroken(kTRUE);

   // embedded canvas
   TRootEmbeddedCanvas *fRootEmbeddedCanvas = new TRootEmbeddedCanvas(0,fHorizontalFrame1,670,590);
   Int_t wfRootEmbeddedCanvas = fRootEmbeddedCanvas->GetCanvasWindowId();
   TCanvas *canvas = new TCanvas("canvas", 10, 10, wfRootEmbeddedCanvas);
   fRootEmbeddedCanvas->AdoptCanvas(canvas);
   fHorizontalFrame1->AddFrame(fRootEmbeddedCanvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   //fRootEmbeddedCanvas->MoveResize(2,2,588,428);

   //fMainFrame->AddFrame(fHorizontalFrame1, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));
   //fHorizontalFrame1->MoveResize(0,0,592,432);

   // horizontal frame
   TGVerticalFrame *fVerticalButtonFrame = new TGVerticalFrame(fHorizontalFrame1,120,600,kVerticalFrame);
   //fHorizontalFrame2->SetLayoutBroken(kTRUE);
   //vector <TGCheckButton *> fTextButtons;
   for (int iButton = 0; iButton<nHistos; iButton++) {
	TGCheckButton * fTextButton =  new TGCheckButton(fVerticalButtonFrame,Histograms.at(iButton)->GetName());
	fTextButton->SetCommand("Plot(canvas)");
   	fVerticalButtonFrame->AddFrame(fTextButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   	fTextButtons.push_back( fTextButton );
	Histograms[iButton]->Draw("same");

   }
   TGButton *b = new TGTextButton(fVerticalButtonFrame, "Quit");
   b->SetCommand("gApplication->Terminate()");
   fVerticalButtonFrame->AddFrame(b, new TGLayoutHints(kLHintsLeft | kLHintsBottom,2,2,2,2));

   fHorizontalFrame1->AddFrame(fVerticalButtonFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   fMainFrame->AddFrame(fHorizontalFrame1, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   //fHorizontalFrame2->MoveResize(592,2,128,430);

   fMainFrame->SetMWMHints(kMWMDecorAll,
                        kMWMFuncAll,
                        kMWMInputModeless);
   fMainFrame->MapSubwindows();

   fMainFrame->Resize(fMainFrame->GetDefaultSize());
   fMainFrame->MapWindow();
}  

