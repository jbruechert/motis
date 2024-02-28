import { useAtom } from "jotai";
import { ReactNode, forwardRef } from "react";
import { NavLink } from "react-router-dom";

import { usePaxMonStatusQuery } from "@/api/paxmon.ts";

import { universeAtom } from "@/data/multiverse.ts";
import { showSimPanelAtom } from "@/data/views";

import { cn } from "@/lib/utils";

interface PageLinkProps {
  active?: boolean;
  onClick?: () => void;
  children: ReactNode;
}

function PageLink({ active, onClick, children }: PageLinkProps): JSX.Element {
  return (
    <button
      type="button"
      className={cn(
        "cursor-pointer rounded-md px-3 py-2 text-sm font-medium",
        active
          ? "bg-db-cool-gray-700 text-white"
          : "text-black hover:bg-db-cool-gray-300",
      )}
      onClick={onClick}
    >
      {children}
    </button>
  );
}

interface MainPageLinkProps {
  to: string;
  children: ReactNode;
}

const MainPageLink = forwardRef<HTMLAnchorElement, MainPageLinkProps>(
  ({ to, children }, ref) => {
    return (
      <NavLink
        ref={ref}
        to={to}
        className={({ isActive }) =>
          cn(
            "cursor-pointer rounded-md px-3 py-2 text-sm font-medium",
            isActive
              ? "bg-db-cool-gray-700 text-white"
              : "text-black hover:bg-db-cool-gray-300",
          )
        }
      >
        {children}
      </NavLink>
    );
  },
);
MainPageLink.displayName = "MainPageLink";

function MainMenu(): ReactNode {
  const [showSimPanel, setShowSimPanel] = useAtom(showSimPanelAtom);
  const [universe] = useAtom(universeAtom);
  const { data: paxmonStatus } = usePaxMonStatusQuery(universe);
  const showCheckData =
    paxmonStatus != undefined && paxmonStatus.has_check_data;

  return (
    <nav className="flex space-x-2">
      <MainPageLink to="trips">Züge</MainPageLink>
      <MainPageLink to="transfers">Umstiege</MainPageLink>
      <MainPageLink to="groups">Reisende</MainPageLink>
      <MainPageLink to="stats">Statistiken</MainPageLink>
      <MainPageLink to="status">Status</MainPageLink>
      {showCheckData && <MainPageLink to="eval">Evaluation</MainPageLink>}
      <PageLink
        active={showSimPanel}
        onClick={() => setShowSimPanel((v) => !v)}
      >
        Simulation
      </PageLink>
    </nav>
  );
}

export default MainMenu;
