import { Popover, Transition } from "@headlessui/react";
import { Cog6ToothIcon } from "@heroicons/react/20/solid";
import { useAtom } from "jotai";
import { Fragment } from "react";

import {
  sectionGraphPlotTypeAtom,
  showLegacyLoadForecastChartAtom,
  showLegacyMeasureTypesAtom,
  showOptimizationDebugLogAtom,
} from "@/data/settings";

import { SectionLoadGraphPlotType } from "@/components/trips/SectionLoadGraph";

import { cn } from "@/lib/utils";

const sectionGraphPlotTypes: {
  plotType: SectionLoadGraphPlotType;
  label: string;
}[] = [
  { plotType: "SimpleBox", label: "Vereinfachter Box-Plot" },
  { plotType: "Box", label: "Box-Plot" },
  { plotType: "Violin", label: "Violin-Plot" },
];

function SectionGraphPlotSettings() {
  const [selectedPlotType, setSelectedPlotType] = useAtom(
    sectionGraphPlotTypeAtom,
  );
  const [showLegacyLoadForecastChart, setShowLegacyLoadForecastChart] = useAtom(
    showLegacyLoadForecastChartAtom,
  );

  return (
    <div className="bg-white p-7">
      Auslastungsgrafik:
      <div className="flex flex-col gap-2 pl-3 pt-2">
        {sectionGraphPlotTypes.map(({ plotType, label }) => (
          <label key={plotType} className="inline-flex items-center gap-2">
            <input
              type="radio"
              name="load-level"
              value={plotType}
              checked={selectedPlotType == plotType}
              onChange={() => setSelectedPlotType(plotType)}
            />
            {label}
          </label>
        ))}
        <label className="inline-flex items-center gap-2">
          <input
            type="checkbox"
            name="legacy-load-forecast-chart"
            checked={showLegacyLoadForecastChart}
            onChange={() => setShowLegacyLoadForecastChart((c) => !c)}
          />
          Alte Auslastungsgrafik anzeigen
        </label>
      </div>
    </div>
  );
}

function MeasureSettings() {
  const [showLegacyMeasureTypes, setShowLegacyMeasureTypes] = useAtom(
    showLegacyMeasureTypesAtom,
  );

  return (
    <div className="bg-white p-7 pt-0">
      Maßnahmen:
      <div className="flex flex-col gap-2 pl-3 pt-2">
        <label className="inline-flex items-center gap-2">
          <input
            type="checkbox"
            name="legacy-measure-types"
            checked={showLegacyMeasureTypes}
            onChange={() => setShowLegacyMeasureTypes((c) => !c)}
          />
          Alte Maßnahmentypen anzeigen
        </label>
      </div>
    </div>
  );
}

function OptimizationSettings() {
  const [showOptimizationDebugLog, setShowOptimizationDebugLog] = useAtom(
    showOptimizationDebugLogAtom,
  );

  return (
    <div className="bg-white p-7 pt-0">
      Maßnahmenoptimierung:
      <div className="flex flex-col gap-2 pl-3 pt-2">
        <label className="inline-flex items-center gap-2">
          <input
            type="checkbox"
            name="optimization-debug-log"
            checked={showOptimizationDebugLog}
            onChange={() => setShowOptimizationDebugLog((c) => !c)}
          />
          Debug-Ausgabe anzeigen
        </label>
      </div>
    </div>
  );
}

function Settings(): JSX.Element {
  return (
    <div className="flex h-full items-center justify-center">
      <Popover className="relative">
        {({ open }) => (
          <>
            <Popover.Button
              className={cn(
                open ? "opacity-100" : "opacity-30 hover:opacity-100",
                "align-center flex justify-center rounded-full bg-white p-2 text-black shadow-sm outline-0 dark:bg-gray-600 dark:text-gray-100",
              )}
            >
              <Cog6ToothIcon className="h-5 w-5" aria-hidden="true" />
            </Popover.Button>
            <Transition
              as={Fragment}
              enter="transition ease-out duration-200"
              enterFrom="opacity-0 translate-y-1"
              enterTo="opacity-100 translate-y-0"
              leave="transition ease-in duration-150"
              leaveFrom="opacity-100 translate-y-0"
              leaveTo="opacity-0 translate-y-1"
            >
              <Popover.Panel className="absolute right-0 z-10 mt-1 w-screen max-w-sm px-4">
                <div className="overflow-hidden rounded-lg shadow-lg ring-1 ring-black ring-opacity-5">
                  <SectionGraphPlotSettings />
                  <MeasureSettings />
                  <OptimizationSettings />
                </div>
              </Popover.Panel>
            </Transition>
          </>
        )}
      </Popover>
    </div>
  );
}

export default Settings;
