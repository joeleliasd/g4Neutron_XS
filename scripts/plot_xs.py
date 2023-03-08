# Script to plot the cross section vs kinetic energy          
# Chris Marshall 21 November 2014

import ROOT
import sys
import math
import optparse

ROOT.gROOT.SetBatch(1)

supported_pdgs = [ 2112 ] #211, 321 ]

lineWidth = 2

# graphs of the various components of the cross section
total = ROOT.TGraph()
elastic = ROOT.TGraph()
inelastic = ROOT.TGraph()

# graphs of exclusive neutron-C cross sections from the MoNA simulation paper
nGamma = ROOT.TGraph()
threeAlpha = ROOT.TGraph()
BeAlpha = ROOT.TGraph()
Bnp = ROOT.TGraph()
Bp = ROOT.TGraph()
Cnn = ROOT.TGraph()
allBoronNot12 = ROOT.TGraph()
allBe = ROOT.TGraph()
allLi = ROOT.TGraph()
multiAlpha = ROOT.TGraph()
other_inelastic = ROOT.TGraph()

# graphs of statistical uncertainties on cross sections
nGammaUnc = ROOT.TGraph()
threeAlphaUnc = ROOT.TGraph()
BeAlphaUnc = ROOT.TGraph()
BnpUnc = ROOT.TGraph()
BpUnc = ROOT.TGraph()
CnnUnc = ROOT.TGraph()

#Uncorrelated statistical uncertainty on per-channel cross sections.
#N.B.: countChannel and countTotal should certainly be correlated
def getStatUnc(countChannel, countTotal, xSecTotal, xSecErr):
  channelUnc = math.sqrt(countChannel)
  countTotalUnc = math.sqrt(countTotal)
  return math.sqrt(xSecTotal**2 * (channelUnc**2 + countTotalUnc**2/countTotal**2) + (countChannel/countTotal*xSecErr)**2) / countTotal

def getMass( material_name ):
  if   material_name == "C":  return 12.0 # carbon                                                
  elif material_name == "Fe": return 55.8452 # iron                                            
  elif material_name == "Pb": return 207.21 # lead                                            
  elif material_name == "Bi": return 208.98040 # bismuth                                            
  elif material_name == "Ni": return 58.6934 # nickel                                       
  elif material_name == "Cu": return 63.546 # copper                                          
  sys.exit( "I don't know about the material %s" % material_name )
  return 0.0

def loop( filename, material_name, incoming ):

  # count the number of each type of interaction
  n_elastic = 0
  n_inelastic = 0
  n_nGamma = 0
  n_threeAlpha = 0
  n_BeAlpha = 0
  n_Bnp = 0
  n_Bp = 0
  n_Cnn = 0
  n_allBoronNot12 = 0
  n_allBe = 0
  n_allLi = 0
  n_multiAlpha = 0

  atomic_mass = getMass( material_name )

  f = ROOT.TFile( filename, "OLD" )
  # nTarget TTree has an entry for each interaction, so we'll loop over it
  T = f.Get("nTarget")

  Na = 6.023 * (10**23)

  # run info TTree has one entry, just get the variables out
  runInfo = f.Get("RunInfo")
  density = 0
  enerPrimGen = 0
  numberEvts = 0
  thickness = 0
  for entry in runInfo:
    density = entry.density # density of material in g/cm3
    enerPrimGen = entry.enerPrimGen # kinetic energy of incoming particle
    numberEvts = entry.numberEvts # total number of events (T.GetEntries() is the number of interactions)
    thickness = entry.tickness # thickness of the target in mm
    break

  for entry in T:

    if entry.intType[0] == 2:
      n_elastic += 1
    else:
      n_inelastic += 1

      # loop over particles to determine if this was an n-gamma production inelastic interaction
      nNeutrons = 0
      nProtons = 0
      nGammas = 0
      nAlphas = 0
      hasC10 = False
      hasC11 = False
      hasC12 = False
      hasBe9 = False
      hasB11 = False
      hasB12 = False
      hasBoronNot12 = False
      hasAnyBe = False
      hasAnyLi = False
      sumNucleons = 0
      mostProtons = 0

      #print "Found an event with:"
      for whichPart in range(entry.npart):
        if entry.IntType[whichPart] == 1: #Only look at inelastic children
          pdgCode = entry.pdg[whichPart]
          #print "a " + str(pdgCode)
          if abs(pdgCode) > 1e4:
            nNucleons = int(abs(pdgCode) % 1e4) / int(10)
            sumNucleons += int(abs(pdgCode) % 1e4) / int(10)
            nNucProtons = int(abs(pdgCode) % 1e7) / int(1e4)
            mostProtons = max(mostProtons, nNucProtons)

            if pdgCode == 1000060120:
              hasC12 = True
            elif pdgCode == 1000060110:
              hasC11 = True
            elif pdgCode ==10000060100: #Added as additional reaction in ND280
              hasC10 = True #Added as additional reaction in ND280
            elif pdgCode == 1000040090: #TODO: I never seem to see 9Be
              hasBe9 = True
              hasAnyBe = True
            elif pdgCode == 1000050110:
              hasB11 = True
              hasBoronNot12 = True
            elif pdgCode == 1000050120:
              hasB12 = True
            elif pdgCode == 1000020040:
              nAlphas += 1
            elif pdgCode == 1000040080:
              nAlphas += 2 #GEANT likes to produce Be8 which decays almost immediately into 2 alphas
            elif nNucProtons == 5 and nNucleons != 12:
              hasBoronNot12 = True
            elif nNucProtons == 4:
              hasAnyBe = True
            elif nNucProtons == 3:
              hasAnyLi = True
            #else:
            #  print "Found another nucleus with PDG code " + str(pdgCode) + ", " + str(int(abs(pdgCode) % 1e7) / int(1e4)) + " protons, and " + str(int(abs(pdgCode) % 1e4) / int(10)) + " nucleons."
          elif pdgCode == 2112:
            sumNucleons += 1
            nNeutrons += 1
          elif pdgCode == 2212:
            sumNucleons += 1
            nProtons += 1
          elif pdgCode == 22:
            nGammas += 1

      #if sumNucleons > 13:
      #  print "Found an event with sumNucleons == " + str(sumNucleons) + ".  It had:"
      #  for whichPart in range(entry.npart):
      #    print "     " + str(entry.pdg[whichPart])

      if hasC12 and nNeutrons == 1 and nGammas > 0:
        n_nGamma += 1
      elif hasC11 and nNeutrons == 2:
        n_Cnn += 1
      elif hasC10
        n_other
      elif hasB12 and nProtons == 1:
        n_Bp += 1
      elif hasB11 and nProtons == 1 and nNeutrons == 1:
        n_Bnp += 1
      elif hasBe9 and nAlphas == 1:
        n_BeAlpha += 1
      elif nAlphas == 3 and nNeutrons == 1:
        n_threeAlpha += 1
      elif (mostProtons == 2 or (mostProtons == 4 and nNucleons == 8)) and sumNucleons == 13:
        n_multiAlpha += 1
      elif sumNucleons == 13 and (not hasBoronNot12) and (not hasAnyBe) and (not hasAnyLi):
        print "Got another type of interaction:"
        for whichPart in range(entry.npart):
          print "a " + str(entry.pdg[whichPart])

      if hasBoronNot12 and sumNucleons == 13:
        n_allBoronNot12 += 1

      if hasAnyBe and sumNucleons == 13:
        n_allBe += 1

      if hasAnyLi and sumNucleons == 13:
        n_allLi += 1

  # convert number of events to a cross section
  surviveNum_total = numberEvts - n_inelastic - n_elastic
  total_int = n_elastic + n_inelastic
  n_other_inelastic = n_inelastic - n_nGamma - n_Cnn - n_Bp - n_threeAlpha - n_allBoronNot12 - n_allBe - n_allLi - n_multiAlpha #- n_BeAlpha - n_Bnp only if not using n_allBoronNot12

  convert_to_cm2_per_C_atom = atomic_mass / (density*Na*thickness*0.1)

  total_XS = math.log(float(numberEvts)/float(surviveNum_total)) * convert_to_cm2_per_C_atom * 10**27 # convert to mb per atom
  error_total_XS = math.sqrt(float(total_int)) * convert_to_cm2_per_C_atom / surviveNum_total * 10**27 # stat error
  inelastic_XS = (float(n_inelastic)/total_int)*total_XS
  elastic_XS = (float(n_elastic)/total_int)*total_XS
  nGamma_XS = (float(n_nGamma)/total_int)*total_XS
  threeAlpha_XS = (float(n_threeAlpha)/total_int)*total_XS
  BeAlpha_XS = (float(n_BeAlpha)/total_int)*total_XS
  Bnp_XS = (float(n_Bnp)/total_int)*total_XS
  Bp_XS = (float(n_Bp)/total_int)*total_XS
  Cnn_XS = (float(n_Cnn)/total_int)*total_XS
  allBoronNot12_XS = (float(n_allBoronNot12)/total_int)*total_XS
  allBe_XS = (float(n_allBe)/total_int)*total_XS
  allLi_XS = (float(n_allLi)/total_int)*total_XS
  multiAlpha_XS = (float(n_multiAlpha)/total_int)*total_XS
  other_inelastic_XS = (float(n_other_inelastic)/total_int)*total_XS

  print "Energy %4.0f MeV, total XS = %2.1f inelastic = %2.1f elastic = %2.1f" % (enerPrimGen, total_XS, inelastic_XS, elastic_XS)

  n = total.GetN()
  total.SetPoint( n, enerPrimGen, total_XS )
  elastic.SetPoint( n, enerPrimGen, elastic_XS )
  inelastic.SetPoint( n, enerPrimGen, inelastic_XS )
  nGamma.SetPoint( n, enerPrimGen, nGamma_XS )
  threeAlpha.SetPoint( n, enerPrimGen, threeAlpha_XS )
  BeAlpha.SetPoint( n, enerPrimGen, BeAlpha_XS )
  Bnp.SetPoint( n, enerPrimGen, Bnp_XS )
  Bp.SetPoint( n, enerPrimGen, Bp_XS )
  Cnn.SetPoint( n, enerPrimGen, Cnn_XS )
  allBoronNot12.SetPoint( n, enerPrimGen, allBoronNot12_XS )
  allBe.SetPoint( n, enerPrimGen, allBe_XS )
  allLi.SetPoint( n, enerPrimGen, allLi_XS )
  multiAlpha.SetPoint( n, enerPrimGen, multiAlpha_XS )
  other_inelastic.SetPoint( n, enerPrimGen, other_inelastic_XS )

  #Store statistical uncertainties
  nGammaUnc.SetPoint(n, enerPrimGen, getStatUnc(n_nGamma, total_int, total_XS, error_total_XS))
  threeAlphaUnc.SetPoint(n, enerPrimGen, getStatUnc(n_threeAlpha, total_int, total_XS, error_total_XS))
  BeAlphaUnc.SetPoint(n, enerPrimGen, getStatUnc(n_BeAlpha, total_int, total_XS, error_total_XS))
  BnpUnc.SetPoint(n, enerPrimGen, getStatUnc(n_Bnp, total_int, total_XS, error_total_XS))
  BpUnc.SetPoint(n, enerPrimGen, getStatUnc(n_Bp, total_int, total_XS, error_total_XS))
  CnnUnc.SetPoint(n, enerPrimGen, getStatUnc(n_Cnn, total_int, total_XS, error_total_XS))

if __name__ == "__main__":
  usage = "%prog [options]\n" \
        + " OPTIONS: \n"\
        + " --material <target>\n"\
        + " --filenames <prefix>\n"\
        + " --nfiles <n>\n"

  parser = optparse.OptionParser(usage=usage)
  parser.add_option("--incoming", dest="incoming", action="store", help="PDG of incoming particle", type="int", default=2112)
  parser.add_option("--material", dest="material", action="store", help="What material are you scattering on", type="string", default="C")
  parser.add_option("--filenames", dest="filenames", action="store", help="Look for files <prefix>_NNNN.root", type="string", default="ntuple")
  parser.add_option("--nfiles", dest="nfiles", action="store", help="How many files are there", type="int", default=0)

  (options, commands) = parser.parse_args()

  material_name = options.material
  nfiles = options.nfiles
  filenames = options.filenames
  incoming = options.incoming

  if incoming not in supported_pdgs:
    print "Can't do incoming pdg of %d, you'll have to modify the script!" % incoming
    sys.exit(0)

  total.SetName("total")
  total.SetTitle(";Incoming KE (MeV);XS (mb)")
  elastic.SetName("elastic")
  elastic.SetTitle(";Incoming KE (MeV);XS (mb)")
  inelastic.SetName("inelastic")
  inelastic.SetTitle(";Incoming KE (MeV);XS (mb)")
  nGamma.SetName("nGamma")
  nGamma.SetTitle(";Incoming KE (MeV);XS (mb)")
  threeAlpha.SetName("threeAlpha")
  threeAlpha.SetTitle(";Incoming KE (MeV);XS (mb)")
  BeAlpha.SetName("BeAlpha")
  BeAlpha.SetTitle(";Incoming KE (MeV);XS (mb)")
  Bnp.SetName("Bnp")
  Bnp.SetTitle(";Incoming KE (MeV);XS (mb)")
  Bp.SetName("Bp")
  Bp.SetTitle(";Incoming KE (MeV);XS (mb)")
  Cnn.SetName("Cnn")
  Cnn.SetTitle(";Incoming KE (MeV);XS (mb)")
  allBoronNot12.SetName("allBoronNot12")
  allBoronNot12.SetTitle(";Incoming KE (MeV);XS (mb)")
  allBe.SetName("allBe")
  allBe.SetTitle(";Incoming KE (MeV);XS (mb)")
  allLi.SetName("allLi")
  allLi.SetTitle(";Incoming KE (MeV);XS (mb)")
  multiAlpha.SetName("multiAlpha")
  multiAlpha.SetTitle(";Incoming KE (MeV);XS (mb)")
  other_inelastic.SetName("other_inelastic")
  other_inelastic.SetTitle(";Incoming KE (MeV);XS (mb)")

  for i in range( nfiles ):
    ntuple = "%s_%04d.root" % (filenames,i)
    loop( ntuple, material_name, incoming )

  # make plots pretty
  elastic.SetLineColor(ROOT.kRed)
  elastic.SetMarkerColor(ROOT.kRed)
  elastic.SetLineWidth(lineWidth)
  inelastic.SetLineColor(ROOT.kBlue)
  inelastic.SetMarkerColor(ROOT.kBlue)
  inelastic.SetLineWidth(lineWidth)
  other_inelastic.SetLineColor(ROOT.kYellow+2)
  other_inelastic.SetMarkerColor(ROOT.kYellow+2)
  other_inelastic.SetLineWidth(lineWidth)
  other_inelastic.SetLineStyle(2)

  nGamma.SetLineColor(ROOT.kRed)
  nGamma.SetMarkerColor(ROOT.kRed)
  nGamma.SetLineStyle(2)
  nGamma.SetLineWidth(lineWidth)
  threeAlpha.SetLineColor(ROOT.kGreen+2)
  threeAlpha.SetMarkerColor(ROOT.kGreen+2)
  threeAlpha.SetLineStyle(2)
  threeAlpha.SetLineWidth(lineWidth)
  BeAlpha.SetLineColor(ROOT.kViolet)
  BeAlpha.SetMarkerColor(ROOT.kViolet)
  BeAlpha.SetLineStyle(2)
  BeAlpha.SetLineWidth(lineWidth)
  Bnp.SetLineColor(ROOT.kOrange)
  Bnp.SetMarkerColor(ROOT.kOrange)
  Bnp.SetLineStyle(2)
  Bnp.SetLineWidth(lineWidth)
  Bp.SetLineColor(ROOT.kBlue)
  Bp.SetMarkerColor(ROOT.kBlue)
  Bp.SetLineStyle(2)
  Bp.SetLineWidth(lineWidth)
  Cnn.SetLineColor(ROOT.kMagenta)
  Cnn.SetMarkerColor(ROOT.kMagenta)
  Cnn.SetLineStyle(2)
  Cnn.SetLineWidth(lineWidth)
  multiAlpha.SetLineColor(ROOT.kPink-6)
  multiAlpha.SetMarkerColor(ROOT.kPink-6)
  multiAlpha.SetLineStyle(2)
  multiAlpha.SetLineWidth(lineWidth)

  #Special cases because these one count other exclusive cross sections too
  allBoronNot12.SetLineColor(ROOT.kBlack)
  allBoronNot12.SetMarkerColor(ROOT.kBlack)
  allBoronNot12.SetLineStyle(3)
  allBoronNot12.SetLineWidth(lineWidth)

  allBe.SetLineColor(ROOT.kCyan+1)
  allBe.SetMarkerColor(ROOT.kCyan+1)
  allBe.SetLineStyle(3)
  allBe.SetLineWidth(lineWidth)

  allLi.SetLineColor(ROOT.kOrange+4)
  allLi.SetMarkerColor(ROOT.kOrange+4)
  allLi.SetLineStyle(3)
  allLi.SetLineWidth(lineWidth)

  total.SetMarkerSize(0.4)
  elastic.SetMarkerSize(0.4)
  inelastic.SetMarkerSize(0.4)
  other_inelastic.SetMarkerSize(0.4)
  nGamma.SetMarkerSize(0.4)
  threeAlpha.SetMarkerSize(0.4)
  BeAlpha.SetMarkerSize(0.4)
  Bnp.SetMarkerSize(0.4)
  Bp.SetMarkerSize(0.4)
  Cnn.SetMarkerSize(0.4)
  allBoronNot12.SetMarkerSize(0.4)
  allBe.SetMarkerSize(0.4)
  allLi.SetMarkerSize(0.4)
  multiAlpha.SetMarkerSize(0.4)

  total.SetMinimum(0.0)
  #total.SetMaximum(800.0)

  leg = ROOT.TLegend( 0.5, 0.6, 0.846, 0.846 )
  leg.AddEntry( total, "Total", "l" )
  leg.AddEntry( elastic, "Elastic", "l" )
  leg.AddEntry( inelastic, "Inelastic", "l" )
  leg.AddEntry( other_inelastic, "Other Inelastic", "l" )
  leg.AddEntry( threeAlpha, "3#alpha", "l" )
  leg.AddEntry( BeAlpha, "^9Be#alpha", "l" )
  leg.AddEntry( Bnp, "^11Bnp", "l" )
  leg.AddEntry( Bp, "^12Bp", "l" )
  leg.AddEntry( nGamma, "n#gamma", "l" )
  leg.AddEntry( Cnn, "^11Cnn", "l" )
  leg.AddEntry( allBoronNot12, "All B except ^12B", "l" )
  leg.AddEntry( allBe, "All Be", "l" )
  leg.AddEntry( allLi, "All Li", "l" )
  leg.AddEntry( multiAlpha, "multi-#alpha", "l" )

  c = ROOT.TCanvas()
  total.Draw("APL")
  nGamma.Draw("PL same")
  threeAlpha.Draw("PL same")
  BeAlpha.Draw("PL same")
  Bnp.Draw("PL same")
  Bp.Draw("PL same")
  Cnn.Draw("PL same")
  allBoronNot12.Draw("PL same")
  allBe.Draw("PL same")
  allLi.Draw("PL same")
  multiAlpha.Draw("PL same")
  elastic.Draw("PL same")
  inelastic.Draw("PL same")
  other_inelastic.Draw("PL same")
  leg.Draw()
  c.SetLogx()
  c.SetLogy()
  c.RedrawAxis()
  c.Print("cross_section.png")

  outFile = ROOT.TFile("cross_section.root", "UPDATE")
  outFile.cd()
  total.Write()
  nGamma.Write()
  threeAlpha.Write()
  BeAlpha.Write()
  Bnp.Write()
  Bp.Write()
  Cnn.Write()
  elastic.Write()
  inelastic.Write()

  nGammaUnc.Write("nGammaUncertainty")
  threeAlphaUnc.Write("threeAlphaUncertainty")
  BeAlphaUnc.Write("BeAlphaUncertainty")
  BnpUnc.Write("BnpUncertainty")
  BpUnc.Write("BpUncertainty")
  CnnUnc.Write("CnnUncertainty")
